// installed on node_modules folder
import exphbs from "express-handlebars";
import flash from "connect-flash";
import indexRoutes from "./paginas/routes/index.routes.js";
import express from "express";
import { dirname, join } from "path";
import { fileURLToPath } from "url";
import session from "express-session";
import axios from "axios";
import { SerialPort } from "serialport";
import { Server } from 'socket.io';
import streamSerialPorts from "./paginas/controllers/seriales.js"
import http from "http";
import { exec } from "child_process";
const PORT = process.env.PORT || 3000;
const sketchPath = './Plantilla/plantilla/plantilla.ino';
const command = `arduino-cli compile --fqbn arduino:avr:uno ${sketchPath}`;
const uploadCommand = `arduino-cli upload -p COM4 --fqbn arduino:avr:uno ${sketchPath}`;
// initializing the app
const app = express();
const server = http.createServer(app);
const io = new Server(server);
const __dirname = dirname(fileURLToPath(import.meta.url));

app.set("views", join(__dirname, "./paginas/views"));


// config view engine
const hbs = exphbs.create({
  defaultLayout: "main",
  layoutsDir: join(app.get("views"), "layouts"),
  partialsDir: join(app.get("views"), "partials"),
  extname: ".hbs",
});
app.engine(".hbs", hbs.engine);
app.set("view engine", ".hbs");
app.use(session({
  secret: 'tu_secreto',
  resave: false,
  saveUninitialized: true
}));

app.use(flash());
// Global Variables
app.use((req, res, next) => {
  res.locals.success_msg = req.flash("success_msg");
  res.locals.success_msg1 = req.flash("success_msg1");
  res.locals.error_msg = req.flash("error_msg");
  res.locals.error = req.flash("error");
  res.locals.console = req.flash("console");
  res.locals.user = req.user;
  next();
});


// routes
app.use(indexRoutes);

app.use(express.static('public'));

app.get('/socket.io/socket.io.js', (req, res) => {
  res.sendFile(__dirname + '/node_modules/socket.io/client-dist/socket.io.js');
});
app.get('/puertos', (req, res) => {
  SerialPort.list()
  .then(ports => {
    res.send({ ports });
  })
  .catch(err => {
    console.error("Error obteniendo la lista de puertos seriales:222", err);
    res.status(500).send("Error obteniendo la lista de puertos seriales2");
  });
 });


app.get('/compile', (req, res) => {
    io.emit('mensaje', 'Verificando codigo......');
  
  
  exec(command, (error, stdout, stderr) => {
    if (error) {
      console.error(`Error al ejecutar el comando: ${error}`);
      res.send(`Error al ejecutar el comando: ${error}`);
      return;
    }
    console.log(`Resultado: ${stdout}`);
    res.send(`Resultado: ${stdout}`);
    io.emit('mensaje', 'Codigo verificado correctamente');
  });
  io.emit('mensaje', 'Compilando...');
  // Ejecutar el comando de carga
  exec(uploadCommand, (error, stdout, stderr) => {
    if (error) {
      console.error('Error al cargar el sketch en la placa:', error);
      io.emit('mensaje', error);
      return;
    }

    if (stderr) {
      console.error('Error al cargar el sketch en la placa:', stderr);
      io.emit('mensaje', stderr);
      return;
    }
    io.emit('mensaje', 'exito');
  });

});





// fo1_oAukrKPwxIj-CPGwRsaCsmYf0QAyeR7hC7Wif3qupNM
server.listen(PORT);

console.log("Express app started on port 3000");