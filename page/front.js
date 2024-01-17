const http = require('http')
const fs = require('fs').promises

http.createServer((req,res)=> {
  if (req.method === 'POST') {
    if (req.url === 'main.html') {
      let body = '';
      req.on('data', data => body += data);
      return req.on('end', () => {
        console.log(body) //foodname=apple&foodsize=2 출력
      })
    }
  }
});