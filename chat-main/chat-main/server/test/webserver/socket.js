const WebSocket=require("ws").Server
const https=require("https").createServer
const fs=require("fs")

const certPath="letsencrypt/PEM"
const server=https({
	ca:fs.readFileSync(`${certPath}/aiv.asuscomm.com-chain.pem`),
	key:fs.readFileSync(`${certPath}/aiv.asuscomm.com-key.pem`),
	cert:fs.readFileSync(`${certPath}/aiv.asuscomm.com-crt.pem`),
	passphrase:"mango"
})
const wss = new WebSocket({ server: server });

wss.on('connection', (ws)=>{
	ws.on('message', (data)=>{
		console.log('received: %s', data);
	});
	ws.send('something');
});
server.listen(2025)
// const {WebSocketServer}=require("ws")

// const wss = new WebSocketServer({ port: 2025 });

// wss.on('connection', (ws)=>{
// 	ws.on('message', (data)=>{
// 		console.log('received: %s', data);
// 	});
// 	ws.send('something');
// });