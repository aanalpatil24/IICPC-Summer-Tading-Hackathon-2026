FROM node:18-alpine
WORKDIR /app
COPY src/leaderboard/package*.json ./
RUN npm install
COPY src/leaderboard/ .
CMD ["node", "server.js"]

```
