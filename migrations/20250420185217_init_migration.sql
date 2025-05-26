-- Add migration script here
CREATE EXTENSION IF NOT EXISTS pgcrypto;

-- Table to store queued songs to process
CREATE TABLE IF NOT EXISTS "songQueue" (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    filename TEXT NOT NULL,
    status TEXT CHECK (status IN ('pending', 'processing', 'done')),
    data BYTEA NULL
);

-- Table to store queued metadata
CREATE TABLE IF NOT EXISTS "metadataQueue" (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    metadata jsonb NOT NULL,
    created_at timestamptz DEFAULT now(),
    song_queue_id UUID NOT NULL
);

-- Table to store queued coverart
CREATE TABLE IF NOT EXISTS "coverartQueue" (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    data BYTEA NOT NULL,
    song_queue_id UUID NULL
);

-- Create an index for better query performance
CREATE INDEX metadata_queue_data_metadata ON "metadataQueue" USING gin (metadata);

-- Table to store a song's info
CREATE TABLE IF NOT EXISTS "song" (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    title TEXT NOT NULL,
    artist TEXT NOT NULL,
    album_artist TEXT NOT NULL,
    album TEXT NOT NULL,
    genre TEXT NOT NULL,
    -- TODO: Address discrepancy of date and year at some point
    -- date TEXT NOT NULL,
    year INT NOT NULL,
    track INT NOT NULL,
    disc INT NOT NULL,
    track_count INT NOT NULL,
    disc_count INT NOT NULL,
    duration INT NOT NULL,
    audio_type TEXT NOT NULL,
    date_created timestamptz DEFAULT now(),
    filename TEXT NOT NULL,
    directory TEXT NOT NULL,
    user_id UUID NULL
    -- TODO: Add coverart id later. This will allow multiple songs to be linked to a single cover art
);

CREATE TABLE IF NOT EXISTS "coverart" (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    title TEXT NOT NULL,
    -- TODO: Separate path later
    path TEXT NOT NULL,
    song_id UUID NOT NULL
    -- TODO: Add type later
);
