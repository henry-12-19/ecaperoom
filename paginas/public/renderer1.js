
// Importa los módulos necesarios de Blockly
function verificar() {
    
    const selectPuertos2 = document.getElementById('console-output');
    document.getElementById("miDiv").style.display = "block";
    selectPuertos2.innerHTML = "consola: " + "&nbsp;" +"Compilando....";
    axios.get('/compile')
      .then((res) => {
        console.log(res.data);
        selectPuertos2.innerHTML = "consola: " + "&nbsp;" +res.data;
        setTimeout(function () {
            document.getElementById("miDiv").style.display = "none";
        }, 5000);
      })
      .catch((err) => {
        console.error(err);
        setTimeout(function () {
            selectPuertos2.innerHTML = "";
            document.getElementById("miDiv").style.display = "none";
        }, 5000);
      });
  };
// Importa los módulos necesarios de Blockly
