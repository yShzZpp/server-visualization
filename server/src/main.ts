import { NestFactory } from '@nestjs/core';
import { AppModule } from './app.module';
import { Client } from 'ssh2';

import SERVER = require('../server.config');

async function bootstrap() {
  const conn = new Client();
  conn
    .on('ready', () => {
      console.log('Client :: ready');
      conn.exec('uptime', (err, stream) => {
        if (err) throw err;
        stream
          .on('close', (code, signal) => {
            console.log(
              'Stream :: close :: code: ' + code + ', signal: ' + signal,
            );
            conn.end();
          })
          .on('data', (data) => {
            console.log('STDOUT: ' + data);
          })
          .stderr.on('data', (data) => {
            console.log('STDERR: ' + data);
          });
      });
    })
    .connect({
      host: SERVER.host,
      port: SERVER.port,
      username: SERVER.username,
      password: SERVER.password,
    });
  const app = await NestFactory.create(AppModule);
  await app.listen(3000);
}
bootstrap();
