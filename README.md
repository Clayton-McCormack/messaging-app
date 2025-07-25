# messaging-app

This repository contains a simple messaging application consisting of a C++ backend and a small React based frontend.

## Server

The server is a very small HTTP API implemented using only the C++ standard library. It exposes two routes:

- `GET /messages` – returns all messages as JSON.
- `POST /messages` – accepts a JSON body containing `username` and `content` fields and stores it in memory.

To build the server run:

```bash
cd server
make
```

Then start it with:

```bash
./server
```

The server listens on port `8080`.

## Frontend

The frontend is located in the `frontend` folder. It uses React via CDN links so no build step is required. Open `frontend/index.html` in a browser while the server is running to test the app.

## Notes

This project is intentionally minimal to demonstrate the basic structure of a messaging website and application.
