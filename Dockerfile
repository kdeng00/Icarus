# Stage 1: Build the application
# Use a specific Rust version for reproducibility. Choose one that matches your development environment.
# Using slim variant for smaller base image
FROM rust:1.88 as builder

# Set the working directory inside the container
WORKDIR /usr/src/app

# Install build dependencies if needed (e.g., for certain crates like sqlx with native TLS)
# RUN apt-get update && apt-get install -y pkg-config libssl-dev

# Install build dependencies if needed (e.g., git for cloning)
RUN apt-get update && apt-get install -y --no-install-recommends \
    pkg-config libssl3 \
    ca-certificates \
    openssh-client git \
    && rm -rf /var/lib/apt/lists/*

# Configure SSH to use the custom port (e.g., 2222)
RUN mkdir -p ~/.ssh && \
    echo "Host git.kundeng.us" >> ~/.ssh/config && \
    echo "    Port 8122" >> ~/.ssh/config && \
    echo "    User git" >> ~/.ssh/config && \
    chmod 600 ~/.ssh/config

# << --- ADD HOST KEY HERE --- >>
# Replace 'yourgithost.com' with the actual hostname (e.g., github.com)
RUN mkdir -p -m 0700 ~/.ssh && \
    ssh-keyscan -p 8122 git.kundeng.us >> ~/.ssh/known_hosts

# Copy Cargo manifests
COPY Cargo.toml Cargo.lock ./

# Build *only* dependencies to leverage Docker cache
# This dummy build caches dependencies as a separate layer
RUN --mount=type=ssh mkdir src && \
    echo "fn main() {println!(\"if you see this, the build broke\")}" > src/main.rs && \
    cargo build --release --quiet && \
    rm -rf src target/release/deps/icarus* # Clean up dummy build artifacts (replace icarus)

# Copy the actual source code
COPY src ./src
# If you have other directories like `templates` or `static`, copy them too
COPY .env ./.env
COPY migrations ./migrations

# << --- SSH MOUNT ADDED HERE --- >>
# Build *only* dependencies to leverage Docker cache
# This dummy build caches dependencies as a separate layer
# Mount the SSH agent socket for this command
RUN --mount=type=ssh \
    cargo build --release --quiet

# Stage 2: Create the final, smaller runtime image
# Use a minimal base image like debian-slim or even distroless for security/size
FROM ubuntu:24.04

# Install runtime dependencies if needed (e.g., SSL certificates)
RUN apt-get update && apt-get install -y ca-certificates libssl-dev libssl3 && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /usr/local/bin

# Copy the compiled binary from the builder stage
# Replace 'icarus' with the actual name of your binary (usually the crate name)
COPY --from=builder /usr/src/app/target/release/icarus .

# Copy other necessary files like .env (if used for runtime config) or static assets
# It's generally better to configure via environment variables in Docker though
COPY --from=builder /usr/src/app/.env .
COPY --from=builder /usr/src/app/migrations ./migrations

# Expose the port your Axum app listens on (e.g., 3000 or 8000)
EXPOSE 3000

# Set the command to run your application
# Ensure this matches the binary name copied above
CMD ["./icarus"]
