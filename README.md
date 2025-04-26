

# Getting Started
## Docker
Make sure `icarus_auth` is located in the root of the parent directory if using docker.

Create a `.env` file for both projects - `icarus_auth` and `icarus` - in the root of each project.

Build containers
```
docker compose build --ssh default api auth_api
```

Bring it up
```
docker compose up -d --force-recreate api auth_api
```
