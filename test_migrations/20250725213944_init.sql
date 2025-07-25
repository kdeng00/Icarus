-- Add migration script here
CREATE EXTENSION IF NOT EXISTS pgcrypto;

-- Table to store queued songs to process
CREATE TABLE IF NOT EXISTS "songQueue" (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    filename TEXT NOT NULL,
    status TEXT CHECK (status IN ('pending', 'ready', 'processing', 'done')),
    data BYTEA NULL,
    user_id UUID NULL
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
    data BYTEA NULL,
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
);


CREATE TABLE IF NOT EXISTS "coverart" (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    title TEXT NOT NULL,
    path TEXT NOT NULL,
    song_id UUID NOT NULL
);

INSERT INTO "song" (id, title, artist, album_artist, album, genre, year, track, disc, track_count, disc_count, duration, audio_type, date_created, filename, directory, user_id) VALUES('44cf7940-34ff-489f-9124-d0ec90a55af9', 'Hypocrite Like The Rest', 'Kuoth', 'Kuoth', 'I', 'Alternative Hip-Hop', 2020, 1, 1, 9, 1, 139, 'flac', '2020-01-01 13:00:00-05', 'track01.flac', 'tests/I', '47491f9b-725a-4ba4-b9a5-711e1be46670');
-- Might not to disable constraints on fields
-- INSERT INTO "song" (id, title, artist, album_artist, album, genre, year, track, disc, track_count, disc_count, duration, audio_type, date_created, filename, directory, user_id) VALUES('44cf7940-34ff-489f-9124-d0ec90a55af9', 'Hypocrite Like The Rest', 'Kuoth', 'Kuoth', 'I', 'Alternative Hip-Hop', 2020, 1, 1, 9, 1, 139, 'flac', '2020-01-01 13:00:00-05', 'track01.flac', 'tests/I', '47491f9b-725a-4ba4-b9a5-711e1be46670');
-- INSERT INTO "coverart" VALUES('', 'I', 'tests/I/Coverart.jpg', '44cf7940-34ff-489f-9124-d0ec90a55af9');
-- Re-enable the constraints on the fields
