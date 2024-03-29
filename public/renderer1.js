const socket = io();

// Importa los módulos necesarios de Blockly
function verificar() {
  axios.get('/compile')
    .then((res) => {
      console.log(res.data);

    })
    .catch((err) => {
      console.error(err);

    });
};

function miFuncion() {
  const selectPuertos = document.getElementById('puertos-select');
  // Limpiar opciones existentes
  selectPuertos.innerHTML = '';
  axios.get('/puertos')
    .then((res) => {
      const data = res.data.ports;
      if (data && Array.isArray(data) && data.length > 0) {
        data.forEach(port => {
          const option = document.createElement('option');
          option.value = port.path;
          if (port.manufacturer && port.productId) {
            // Identificar por fabricante y producto (ID de producto del chip USB)
            if (port.manufacturer.includes('Arduino') && port.productId.includes('0043')) {
              console.log(port.path, 'Arduino Uno');
              option.textContent = `Puerto: ${port.path}, Id: Arduino Uno`;
            } else if (port.manufacturer.includes('Arduino') && port.productId.includes('0042')) {
              console.log(port.path, 'Arduino Mega');
              option.textContent = `Puerto: ${port.path}, Id: Arduino Mega`;
            }
            else if (port.productId == 7523) {
              // Identificar por nombre del fabricante (genérico)
              console.log(port.productId, 'Arduino Mega - Escape_room');
              option.textContent = `Puerto: ${port.path}, Id: Arduino Mega - Escape_room`;
            }
          } else {
            // Identificar por nombre del fabricante (genérico)
            console.log(port.productId, 'Arduino Genérico');
          }


          selectPuertos.appendChild(option);
        });
      } else {
        const option = document.createElement('option');
        option.value = `No hay dispositivos conectados`;
        option.textContent = `No hay dispositivos conectados`;
        selectPuertos.appendChild(option);
      }
    })
    .catch((err) => {
      console.error(err);

    });
}

// Llamar a setInterval() para ejecutar la función cada 5 segundos (5000 milisegundos)
setInterval(miFuncion, 2000);
socket.on('mensaje', (mensaje) => {
  const selectPuertos2 = document.getElementById('console-output');
  document.getElementById("miDiv").style.display = "block";
  selectPuertos2.innerHTML = mensaje;
  setTimeout(function () {
    document.getElementById("miDiv").style.display = "none";
  }, 10000);
});
// Importa los módulos necesarios de Blockly
