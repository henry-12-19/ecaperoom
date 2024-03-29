
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
  const socket = io();

  socket.on('mensaje', (mensaje) => {
    const selectPuertos2 = document.getElementById('console-output');
    document.getElementById("miDiv").style.display = "block";
    selectPuertos2.innerHTML = mensaje;
    setTimeout(function () {
      document.getElementById("miDiv").style.display = "none";
  }, 5000);
  });
// Importa los módulos necesarios de Blockly
