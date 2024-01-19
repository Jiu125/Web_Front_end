const express=require("express")
const https=require("https")
const path=require("path")
const fs=require("fs")
const app=express()

const dir=path.resolve()
const certPath="letsencrypt/PEM"
const sslOptions={
	ca:fs.readFileSync(`${certPath}/aiv.asuscomm.com-chain.pem`),
	key:fs.readFileSync(`${certPath}/aiv.asuscomm.com-key.pem`),
	cert:fs.readFileSync(`${certPath}/aiv.asuscomm.com-crt.pem`),
	// ca:fs.readFileSync('cert/ca.crt'),
	// key:fs.readFileSync('cert/private.key'),
	// cert:fs.readFileSync('cert/private.crt'),
	passphrase:"mango"
}

app.get("/",(req,res)=>{
	res.sendFile(`${dir}/main.html`)
})
app.get('*',(req,res)=>{
	console.log(req.params[0]) 
	res.sendFile(`${dir}${req.params[0]}`)
	// res.send("hello")
})

app.listen(80,()=>{
	console.log("server at port 80")
})
https.createServer(sslOptions,app,(req,res)=>{
	console.log("여긴 뭐지?")
}).listen(443,()=>{
	console.log("server at port 443")
})