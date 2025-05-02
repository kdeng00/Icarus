-- Add migration script here
CREATE EXTENSION IF NOT EXISTS pgcrypto;

-- Table to store queued songs to process
CREATE TABLE IF NOT EXISTS "songQueue" (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    filename TEXT NOT NULL,
    status TEXT CHECK (status IN ('pending', 'processing', 'done')),
    data BYTEA NOT NULL
);

-- Table to store queued metadata
CREATE TABLE IF NOT EXISTS "metadataQueue" (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    metadata jsonb NOT NULL,
    created_at timestamptz DEFAULT now(),
    song_queue_id UUID NOT NULL
);

-- Table to store queued coverart
CREATE TABLE IF NOT EXISTS "coverartQueue" {
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    data BYTEA NOT NULL,
    song_queue_id UUID NOT NULL
};

-- Create an index for better query performance
CREATE INDEX metadata_queue_data_metadata ON "metadataQueue" USING gin (metadata);
