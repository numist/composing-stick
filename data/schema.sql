DROP TABLE IF EXISTS post_index;
CREATE TABLE IF NOT EXISTS post_index (
        location TEXT NOT NULL, -- corresponds to path to postdata on disk
        time INTEGER,           -- strtotime of timestamp.txt
        series TEXT             -- colon-separated list of series this post belongs to
);