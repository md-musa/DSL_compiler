SELECT timestamp, url
FROM sample.log
WHERE url MATCH "/api/v1/auth/.*"
