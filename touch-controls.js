window.createTouchControls = ({ canvas, touchControls, touchButtons }) => {
    let enabled = false;
    let touchRegistered = false;

    const activeReleaseTimers = new Map();

    const keyMap = {
        ArrowUp: { key: "ArrowUp", code: "ArrowUp", keyCode: 38, which: 38 },
        ArrowDown: { key: "ArrowDown", code: "ArrowDown", keyCode: 40, which: 40 },
        ArrowLeft: { key: "ArrowLeft", code: "ArrowLeft", keyCode: 37, which: 37 },
        ArrowRight: { key: "ArrowRight", code: "ArrowRight", keyCode: 39, which: 39 },
        KeyG: { key: "g", code: "KeyG", keyCode: 71, which: 71 },
        KeyP: { key: "p", code: "KeyP", keyCode: 80, which: 80 },
        KeyN: { key: "n", code: "KeyN", keyCode: 78, which: 78 },
        KeyR: { key: "r", code: "KeyR", keyCode: 82, which: 82 },
        KeyQ: { key: "q", code: "KeyQ", keyCode: 81, which: 81 }
    };

    const updateVisibility = () => {
        touchControls.classList.toggle("visible", enabled && touchRegistered);
    };

    const registerTouch = () => {
        if (touchRegistered) {
            return;
        }

        touchRegistered = true;
        updateVisibility();
    };

    const patchKeyboardEvent = (event, binding) => {
        try {
            Object.defineProperty(event, "keyCode", { get: () => binding.keyCode });
            Object.defineProperty(event, "which", { get: () => binding.which });
            Object.defineProperty(event, "charCode", { get: () => 0 });
        } catch {
        }
    };

    const dispatchSyntheticKeyEvent = (type, binding) => {
        const event = new KeyboardEvent(type, {
            key: binding.key,
            code: binding.code,
            bubbles: true,
            cancelable: true
        });

        patchKeyboardEvent(event, binding);
        canvas.dispatchEvent(event);
    };

    const pressVirtualKey = (keyName) => {
        const binding = keyMap[keyName];
        if (!binding) {
            return;
        }

        canvas.focus();
        dispatchSyntheticKeyEvent("keydown", binding);
    };

    const releaseVirtualKey = (keyName) => {
        const binding = keyMap[keyName];
        if (!binding) {
            return;
        }

        dispatchSyntheticKeyEvent("keyup", binding);
    };

    const tapVirtualKey = (keyName, duration = 55) => {
        const releaseTimer = activeReleaseTimers.get(keyName);
        if (releaseTimer) {
            window.clearTimeout(releaseTimer);
            activeReleaseTimers.delete(keyName);
        }

        pressVirtualKey(keyName);

        const timerId = window.setTimeout(() => {
            releaseVirtualKey(keyName);
            activeReleaseTimers.delete(keyName);
        }, duration);

        activeReleaseTimers.set(keyName, timerId);
    };

    window.addEventListener("pointerdown", (event) => {
        if (event.pointerType === "touch") {
            registerTouch();
        }
    }, { passive: true });

    window.addEventListener("touchstart", registerTouch, { passive: true });

    touchButtons.forEach((button) => {
        const keyName = button.dataset.key;

        button.addEventListener("pointerdown", (event) => {
            event.preventDefault();
            registerTouch();
            canvas.focus();

            if (typeof button.setPointerCapture === "function") {
                button.setPointerCapture(event.pointerId);
            }

            button.classList.add("touch-active");
            tapVirtualKey(keyName);
        });

        const releaseButton = (event) => {
            event.preventDefault();
            button.classList.remove("touch-active");
        };

        button.addEventListener("pointerup", releaseButton);
        button.addEventListener("pointercancel", releaseButton);
        button.addEventListener("lostpointercapture", releaseButton);
    });

    updateVisibility();

    return {
        setEnabled(value) {
            enabled = value;
            updateVisibility();
        }
    };
};
