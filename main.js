const { app, BrowserWindow, ipcMain, dialog, Notification } = require('electron');
const fs = require('fs');
const path = require('path');
const { exec } = require('child_process');
const SerialPort = require('serialport');
let mainWindow;



app.on('ready', () => {
  mainWindow = new BrowserWindow({
    nodeIntegration: true, // Desactiva la integración de Node.js en la ventana
    contextIsolation: true, // Aísla el contexto del contenido de la página
    webPreferences: {
      preload: path.join(__dirname, 'preload.js')
    }
  });
  mainWindow.loadFile('paginas/index.html');
  mainWindow.maximize();
  const NOTIFICATION_TITLE = 'Basic Notification'
  const NOTIFICATION_BODY = 'Notification from the Main process'

  function showNotification() {
    new Notification({ title: NOTIFICATION_TITLE, body: NOTIFICATION_BODY }).show()
  }
  app.whenReady().then(showNotification)
});



//*************************************************************************************************************** */
const obtenerPuertos = async () => {
  try {
    const ports = await SerialPort.SerialPort.list();

    if (ports.length > 0) {
      mainWindow.webContents.send("fromMain", ports);

    } else {
      mainWindow.webContents.send("fromMain", "no hay puertos");

    }
  } catch (error) {
    console.error('Error al listar los puertos seriales:', error);
  }
};

// Llamar a la función cada cierto intervalo de tiempo (por ejemplo, cada 5 segundos)
setInterval(obtenerPuertos, 500);

//************************************************************************************************************************** */
ipcMain.on('generar-y-guardar-codigo', (event, datos) => {

  const { pins, tiempo } = datos;
  const plantillaPath = path.join(__dirname, 'Plantilla/plantilla/plantilla.ino');

  fs.readFile(plantillaPath, 'utf8', (err, data) => {
    if (err) {
      console.error('Error al leer la plantilla:', err);
      return;
    }
    let nuevoCodigo = data;
    const numPines = pins.length;
    const datos2 = leerDatos();
    if (datos2) {
      // Crear un array de arrays
      let arrays = [];
      // Definir el número de arrays que deseas y el número de elementos en cada uno
      const numArrays = 5;
      const numElementos = 5;

      // Llenar el array de arrays con arrays vacíos
      for (let i = 0; i < numArrays; i++) {
        arrays.push([]);
      }
      // Llenar los arrays internos con datos usando un bucle for anidado
      for (let i = 0; i < numArrays; i++) {
        const reto = datos2.retos[i];
        for (let j = 0; j < numElementos; j++) {
          if (i == 0) {
            arrays[i].push(`"` + `${reto['n' + (j + 1)]}` + `"`);
          } else if (i == 3) {
            arrays[i].push(`B` + `${reto['n' + (j + 1)]}`);
          } else {
            arrays[i].push(`${reto['n' + (j + 1)]}`);
          }

        }
      }
      // Imprimir los arrays llenos
      arrays.forEach((array, index) => {
        const regex = new RegExp(`{{resp${index + 1}}}`, 'g');
        nuevoCodigo = nuevoCodigo.replace(regex, array);
      });
    }

    if (tiempo) {
      const regex = new RegExp(`{{tiempo}}`, 'g');
      nuevoCodigo = nuevoCodigo.replace(regex, tiempo);
    }

    for (let index = 0; index < numPines; index++) {
      const regex = new RegExp(`{{pin${index + 1}}}`, 'g');
      nuevoCodigo = nuevoCodigo.replace(regex, "while(jugando==true){ " + '\n' + pins[index] + '\n' + " delay(500); } " + '\n' + "jugando=true;");
    }

    // Si hay menos de 5 pines, reemplaza los marcadores restantes con un texto personalizado
    for (let index = numPines; index < 6; index++) {
      const regex = new RegExp(`{{pin${index + 1}}}`, 'g');
      nuevoCodigo = nuevoCodigo.replace(regex, `//reto ${index + 1}`);
    }

    dialog.showSaveDialog({
      title: 'Guardar Código',
      defaultPath: 'codigo_Escape_room.ino',
      filters: [{ name: 'Archivos de Arduino', extensions: ['ino'] }]
    }).then(result => {
      if (!result.canceled) {
        const nuevoArchivoPath = result.filePath;
        fs.writeFile(nuevoArchivoPath, nuevoCodigo, 'utf8', (err) => {
          if (err) {
            console.error('Error al guardar el nuevo código:', err);
            return;
          }
          console.log('Nuevo código guardado correctamente en:', nuevoArchivoPath);
        });
      }
    }).catch(err => {
      console.error('Error al mostrar el cuadro de diálogo de guardar:', err);
    });
  });
});
//***************************************************************************************************** */
// Leer y modificar datos del archivo JSON
const filePath = path.resolve(__dirname, 'paginas/database/datos.json');

function leerDatos() {
  try {
    const data = fs.readFileSync(filePath, 'utf8');
    return JSON.parse(data);
  } catch (error) {
    console.error('Error al leer el archivo JSON:', error);
    return null;
  }
}
//******************************************************************************************************** */
function escribirDatos(datos) {
  try {
    fs.writeFileSync(filePath, JSON.stringify(datos, null, 2), 'utf8');
    console.log('Datos guardados correctamente.');
    return true;
  } catch (error) {
    console.error('Error al escribir en el archivo JSON:', error);
    return false;
  }
}
//************************************************************************************************************************************* */

ipcMain.on('obtener-datos', (event) => {
  const datos = leerDatos();
  event.reply('datos', datos);
});
//**************************************************** */
ipcMain.on('modificadb', (event, datos) => {
  const { toggleStates, respuestas } = datos;
  const datos2 = leerDatos();
  if (datos2) {
    // Imprimir los arrays llenos
    respuestas.forEach((array, index) => {
      if (index != 3) {
        datos2.retos[index].n1 = array[0];
        datos2.retos[index].n2 = array[1];
        datos2.retos[index].n3 = array[2];
        datos2.retos[index].n4 = array[3];
        datos2.retos[index].n5 = array[4];
      }

    });
    console.log(toggleStates)
    datos2.retos[3].n1 = toggleStates[0];
    datos2.retos[3].n2 = toggleStates[1];
    datos2.retos[3].n3 = toggleStates[2];
    datos2.retos[3].n4 = toggleStates[3];
    datos2.retos[3].n5 = toggleStates[4];
    let estado = escribirDatos(datos2);
    if (estado == true) {
      event.reply("result3", "exito");
    }
  }
});
//**************************************************************************************************************************************** */
ipcMain.on('generar-y-guardar-codigo-temp', (event, datos) => {
  const { pins, puerto, placa, tiempo } = datos;
  const plantillaPath = 'temp/plantilla/plantilla.ino';

  fs.readFile(plantillaPath, 'utf8', (err, data) => {
    if (err) {
      console.error('Error al leer la plantilla:', err);
      return;
    }
    let nuevoCodigo = data;
    const numPines = pins.length;
    const datos2 = leerDatos();
    if (datos2) {
      // Crear un array de arrays
      let arrays = [];
      // Definir el número de arrays que deseas y el número de elementos en cada uno
      const numArrays = 5;
      const numElementos = 5;

      // Llenar el array de arrays con arrays vacíos
      for (let i = 0; i < numArrays; i++) {
        arrays.push([]);
      }
      // Llenar los arrays internos con datos usando un bucle for anidado
      for (let i = 0; i < numArrays; i++) {
        const reto = datos2.retos[i];
        for (let j = 0; j < numElementos; j++) {
          if (i == 0) {
            arrays[i].push(`"` + `${reto['n' + (j + 1)]}` + `"`);
          } else if (i == 3) {
            arrays[i].push(`B` + `${reto['n' + (j + 1)]}`);
          } else {
            arrays[i].push(`${reto['n' + (j + 1)]}`);
          }

        }
      }
      // Imprimir los arrays llenos
      arrays.forEach((array, index) => {
        const regex = new RegExp(`{{resp${index + 1}}}`, 'g');
        nuevoCodigo = nuevoCodigo.replace(regex, array);
      });
    }

    if (tiempo) {
      const regex = new RegExp(`{{tiempo}}`, 'g');
      nuevoCodigo = nuevoCodigo.replace(regex, tiempo);
    }

    for (let index = 0; index < numPines; index++) {
      const regex = new RegExp(`{{pin${index + 1}}}`, 'g');
      nuevoCodigo = nuevoCodigo.replace(regex, "jugando=true;"+'\n' + "while(jugando==true){ " + '\n' + pins[index] + '\n' + " delay(500); } ");
    }

    // Si hay menos de 5 pines, reemplaza los marcadores restantes con un texto personalizado
    for (let index = numPines; index < 6; index++) {
      const regex = new RegExp(`{{pin${index + 1}}}`, 'g');
      nuevoCodigo = nuevoCodigo.replace(regex, `//reto ${index + 1}`);
    }

    const nuevoArchivoPath = 'temp/temino/temino.ino';
    fs.writeFile(nuevoArchivoPath, nuevoCodigo, 'utf8', (err) => {
      if (err) {
        console.error('Error al guardar el nuevo código:', err);
        return;
      }
      console.log('Nuevo código guardado correctamente en:', nuevoArchivoPath);
      mainWindow.webContents.send("result", "Nuevo código guardado correctamente");
    });
  });

  // Ruta al directorio donde se encuentra tu sketch de Arduino
  const sketchDirectory = 'temp/temino/temino.ino';
  const compileCommand = `arduino-cli compile --fqbn arduino:avr:${placa} ${sketchDirectory}`;
  const uploadCommand = `arduino-cli upload -p ${puerto} --fqbn arduino:avr:${placa} ${sketchDirectory}`;
  mainWindow.webContents.send("result", "Cargando código ....");

  exec(compileCommand, (error, stdout, stderr) => {
    if (stdout) {
      mainWindow.webContents.send("result", stdout);
    }

    if (error) {

      console.error('Error al compilar el sketch:', error);
      mainWindow.webContents.send("result", error);
      return;
    }

    if (stderr) {

      console.error('Error de compilación:', stderr);
      mainWindow.webContents.send("result", stderr);
      return;
    }

    console.log('Sketch compilado correctamente. Archivo .hex generado.');
    // Mostrar alerta después de que se ha compilado correctamente

    // Ejecutar el comando de carga
    exec(uploadCommand, (error, stdout, stderr) => {
      if (error) {
        console.error('Error al cargar el sketch en la placa:', error);
        mainWindow.webContents.send("result", error);
        return;
      }

      if (stderr) {
        console.error('Error al cargar el sketch en la placa:', stderr);
        mainWindow.webContents.send("result", stderr);
        return;
      }

      mainWindow.webContents.send("result", 'Sketch cargado correctamente en la placa.');
      dialog.showMessageBox({
        type: 'info',
        message: 'El codigo se ha cargado correctamente en la placa.',
        title: 'Carga exitosa'
      });
    });
  });


});


