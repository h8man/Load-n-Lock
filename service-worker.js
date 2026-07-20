const CACHE_NAME = "load-and-lock-v11";

const CORE_ASSETS = [
    "./",
    "index.html",
    "index.js",
    "index.wasm",
    "index.data",
    "touch-controls.js",
    "manifest.webmanifest",
    "L&L.png",
    "L&L.ico",
    "sprites.png"
];

self.addEventListener("install", (event) => {
    event.waitUntil((async () => {
        const cache = await caches.open(CACHE_NAME);
        await Promise.allSettled(CORE_ASSETS.map((asset) => cache.add(asset)));
        await self.skipWaiting();
    })());
});

self.addEventListener("activate", (event) => {
    event.waitUntil((async () => {
        const cacheNames = await caches.keys();
        await Promise.all(
            cacheNames
                .filter((cacheName) => cacheName !== CACHE_NAME)
                .map((cacheName) => caches.delete(cacheName))
        );
        await self.clients.claim();
    })());
});

self.addEventListener("fetch", (event) => {
    const { request } = event;

    if (request.method !== "GET") {
        return;
    }

    const requestUrl = new URL(request.url);
    if (requestUrl.origin !== self.location.origin) {
        return;
    }

    event.respondWith((async () => {
        const cachedResponse = await caches.match(request);
        if (cachedResponse) {
            return cachedResponse;
        }

        const networkResponse = await fetch(request);
        if (!networkResponse || networkResponse.status !== 200 || networkResponse.type === "opaque") {
            return networkResponse;
        }

        const cache = await caches.open(CACHE_NAME);
        cache.put(request, networkResponse.clone());
        return networkResponse;
    })());
});
