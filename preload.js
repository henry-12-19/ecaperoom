const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('api', {
  send2: (channel, data) => {
    ipcRenderer.send(channel, data);
  }
});
contextBridge.exposeInMainWorld('db', {
  senddb: (channel, data) => {
    ipcRenderer.send(channel, data);
  },
  receive3: (channel, func) => {
   let validChannels = ["result3"];
   if (validChannels.includes(channel)) {
       // Deliberately strip event as it includes `sender` 
       ipcRenderer.on(channel, (event, ...args) => func(...args));
   }
}
});
contextBridge.exposeInMainWorld('api2', {
  send3: (channel, data) => {
    ipcRenderer.send(channel, data);
  },
   receive2: (channel, func) => {
    let validChannels = ["result"];
    if (validChannels.includes(channel)) {
        // Deliberately strip event as it includes `sender` 
        ipcRenderer.on(channel, (event, ...args) => func(...args));
    }
}
});
contextBridge.exposeInMainWorld('coms', {
  com: (channel, data) => {
    ipcRenderer.send(channel, data); 
},
   receive: (channel, func) => {
    let validChannels = ["fromMain"];
    if (validChannels.includes(channel)) {
        // Deliberately strip event as it includes `sender` 
        ipcRenderer.on(channel, (event, ...args) => func(...args));
    }
}
});
contextBridge.exposeInMainWorld('electron', {
  ipcRenderer: ipcRenderer
});

contextBridge.exposeInMainWorld('datosAPI', {
  obtenerDatos: () => {
    return new Promise((resolve, reject) => {
      ipcRenderer.send('obtener-datos');
      ipcRenderer.once('datos', (event, datos) => {
        resolve(datos);
      });
    });
  }
});


