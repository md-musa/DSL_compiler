SELECT timestamp, url
FROM api_errors.log
WHERE url MATCH "/api/v1/auth/.*"
