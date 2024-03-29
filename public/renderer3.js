async function cargarDatos() {
  try {
    const datos = await window.datosAPI.obtenerDatos();

    for (let j = 1; j <= 5; j++) {
      const reto = datos.retos[j - 1];
      for (let i = 1; i <= 5; i++) {

        if (j == 4) {
          var estadoToggle = `${reto['n' + i]}`;
          // Obtener los toggle-switches por su ID
          var toggleSwitch1 = document.getElementById('st1g' + i);
          var toggleSwitch2 = document.getElementById('st2g' + i);
          var toggleSwitch3 = document.getElementById('st3g' + i);
          console.log('st3g' + i);

          toggleSwitch1.checked = (estadoToggle.charAt(0) === "0"); // true si el primer carácter es "0", false en caso contrario
          toggleSwitch2.checked = (estadoToggle.charAt(1) === "0"); // true si el segundo carácter es "0", false en caso contrario
          toggleSwitch3.checked = (estadoToggle.charAt(2) === "0"); // true si el tercer carácter es "0", false en caso contrario

        } else {
          document.getElementById('g' + i + 'r' + j).value = `${reto['n' + i]}`;
        }
      }
    }
  } catch (error) {
    console.error('Error al cargar los datos:', error);
  }
}
function guardardatos() {
  const toggleStates = [];
  let respuestas = [];
  // Definir el número de arrays que deseas y el número de elementos en cada uno
  const numArrays = 5;

  // Llenar el array de arrays con arrays vacíos
  for (let i = 0; i < numArrays; i++) {
    respuestas.push([]);
  }

  // Obtener el estado de los toggle switches
  for (let i = 1; i <= 5; i++) {
    const toggle1 = document.getElementById('st1g' + i).checked ? 0 : 1;
    const toggle2 = document.getElementById('st2g' + i).checked ? 0 : 1;
    const toggle3 = document.getElementById('st3g' + i).checked ? 0 : 1;
    if (i != 4) {
      for(let j=1;j<=5;j++){
        const res1 = document.getElementById('g'+j+'r' + i).value;
        const response = res1.toString() ;
        respuestas[(i-1)].push(response);
      }
      
    }

    // Construir el número binario
    const binaryNumber = toggle1.toString() + toggle2.toString() + toggle3.toString();
    toggleStates.push(binaryNumber);
  }
  console.log(respuestas);
  const data = { toggleStates, respuestas};
    window.db.senddb('modificadb', data);
}
cargarDatos();


// Obtener todos los IDs de los campos de entrada
const ids = ['g1r3', 'g2r3', 'g3r3', 'g4r3', 'g5r3'];

// Agregar un listener de eventos para cada campo de entrada
ids.forEach(id => {
    const input = document.getElementById(id);
    input.addEventListener('input', function(event) {
        // Obtener el valor actual del campo de entrada
        let valor = event.target.value;

        // Reemplazar cualquier carácter que no sea un número con una cadena vacía
        valor = valor.replace(/\D/g, '');

        // Limitar la longitud del valor a 4 caracteres
        valor = valor.substring(0, 4);

        // Actualizar el valor del campo de entrada con el valor modificado
        event.target.value = valor;
    });
});
// Obtener todos los IDs de los campos de entrada
const ids2 = ['g1r1', 'g2r1', 'g3r1', 'g4r1', 'g5r1'];

// Agregar un listener de eventos para cada campo de entrada
ids2.forEach(id => {
    const input = document.getElementById(id);
    input.addEventListener('input', function(event) {
        // Obtener el valor actual del campo de entrada
        let valor = event.target.value;

        // Reemplazar cualquier carácter que no sea una letra con una cadena vacía
        valor = valor.replace(/[^a-zA-Z]/g, '');

        // Actualizar el valor del campo de entrada con el valor modificado
        event.target.value = valor;
    });
});