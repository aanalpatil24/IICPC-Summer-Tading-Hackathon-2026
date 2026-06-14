const express = require('express');
const { createClient } = require('redis');
const { WebSocketServer } = require('ws');
const http = require('http');

const app = express();
const server = http.createServer(app);
const wss = new WebSocketServer({ server });

const redisUrl = `redis://${process.env.REDIS_HOST || 'localhost'}:6379`;
const redisClient = createClient({ url: redisUrl });
redisClient.connect().catch(console.error);

app.use(express.static('public'));

setInterval(async () => {
    try {
        const topTeams = await redisClient.zRangeWithScores('leaderboard', 0, 49, { REV: true });
        const payload = JSON.stringify({ type: 'update', data: topTeams });
        wss.clients.forEach(client => {
            if (client.readyState === 1) client.send(payload);
        });
    } catch (e) {
        console.error("Redis fetch error:", e);
    }
}, 1000);

server.listen(8080, () => {
    console.log('Leaderboard Service running on http://localhost:8080');
});

