function allowDrop(ev) {
    ev.preventDefault();
}

function drag(ev) {
    if (ev.target.classList.contains('bloque')) {
        ev.dataTransfer.setData("text", ev.target.id);
        // Obtener el código asociado al bloque y almacenarlo en la transferencia de datos
        var codigo = ev.target.getAttribute('data-codigo');
        ev.dataTransfer.setData("codigo", codigo);
    }
}

function drop(ev) {
    ev.preventDefault();
    var data = ev.dataTransfer.getData("text");
    var codigo = ev.dataTransfer.getData("codigo");
    var target = ev.target;

    // Verificar si el contenedor ya tiene un bloque dentro
    if (target.querySelector('.bloque') === null) {
        var clone = document.getElementById(data).cloneNode(true);
        target.appendChild(clone);
        clone.setAttribute("draggable", "false");
        // Agregar el evento de doble clic para editar el bloque de código

        // Asociar el fragmento de código al div clonado
        clone.setAttribute("data-codigo", codigo);
    }
}

function eliminarBloque(ev) {

    var bloque = ev.target;
    if (bloque.classList.contains('bloque')) {
        bloque.parentNode.removeChild(bloque);
    }
}


function generarCodigo() {
    var div2 = document.getElementById("div2");
    var tiempo = document.getElementById("timer").value;
    var bloques = div2.getElementsByClassName("bloque");
    var pins = [];

    // Obtener fragmentos de código de cada bloque
    for (var i = 0; i < bloques.length; i++) {
        var bloque = bloques[i];
        var pin = bloque.getAttribute("data-codigo");
        pins.push(pin);
    }

    // Enviar los fragmentos de código al proceso principal para que se genere y guarde el código
    const data = { pins, tiempo };
    window.api.send2('generar-y-guardar-codigo', data);
}


function cargarCodigo() {
    var tiempo = document.getElementById("timer").value;
    var div2 = document.getElementById("div2");
    var bloques = div2.getElementsByClassName("bloque");
    var placa = document.getElementById('placa-select').value;
    var puerto = document.getElementById('puertos-select').value;
    console.log(puerto)
    var pins = [];

    // Obtener fragmentos de código de cada bloque
    for (var i = 0; i < bloques.length; i++) {
        var bloque = bloques[i];
        var pin = bloque.getAttribute("data-codigo");
        pins.push(pin);
    }

    // Enviar los fragmentos de código al proceso principal para que se genere y guarde el código
    const data = { pins, puerto, placa, tiempo };
    window.api2.send3('generar-y-guardar-codigo-temp', data);
}

window.coms.receive("fromMain", (data) => {
    const selectPuertos = document.getElementById('puertos-select');
    // Limpiar opciones existentes
    selectPuertos.innerHTML = '';
    // Crear opciones para cada puerto
    if (data != "no hay puertos") {
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
});


window.api2.receive2("result", (data) => {

    const selectPuertos2 = document.getElementById('console-output');
    if (data) {

        document.getElementById("miDiv").style.display = "block";
        selectPuertos2.innerHTML = "consola: " + "&nbsp;" + data;
        // Crear opciones para cada puerto
        setTimeout(function () {
            document.getElementById("miDiv").style.display = "none";
        }, 15000);
    }
    else {
        setTimeout(function () {
            document.getElementById("miDiv").style.display = "none";
        }, 5000);
    }
});


window.db.receive3("result3", (data) => {
    console.log("holisss")
    if (data == "exito") {
        // Obtener el div al que deseas agregar el código HTML
        const divDestino = document.getElementById('alerta');
        divDestino.style.display = "block";
        // Crear un elemento div para la alerta
        const alertBox = document.createElement('div');
        alertBox.classList.add('alert-box', 'success-alert');

        // Crear el contenido de la alerta
        const alertDiv = document.createElement('div');
        alertDiv.classList.add('alert');

        const alertHeading = document.createElement('h4');
        alertHeading.classList.add('alert-heading');
        alertHeading.textContent = 'EXITO';

        const alertText = document.createElement('p');
        alertText.classList.add('text-medium');
        alertText.textContent = 'Los datos fueron guardados correctamente';

        // Agregar los elementos al div de la alerta
        alertDiv.appendChild(alertHeading);
        alertDiv.appendChild(alertText);
        alertBox.appendChild(alertDiv);

        // Agregar la alerta al div destino
        divDestino.appendChild(alertBox);
        setTimeout(() => {
            divDestino.style.display = 'none';
            alertBox.parentNode.removeChild(alertBox);
        }, 5000);
    }
});