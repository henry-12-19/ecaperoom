import SerialPort from 'serialport';

 const streamSerialPorts = (req, res) => {
  res.setHeader('Content-Type', 'text/event-stream');
  res.setHeader('Cache-Control', 'no-cache');
  res.setHeader('Connection', 'keep-alive');

  const sendPorts = (ports) => {
    res.write(`data: ${JSON.stringify(ports)}\n\n`);
  };

  SerialPort.list().then(ports => {
    sendPorts(ports);
  });

  const intervalId = setInterval(() => {
    SerialPort.list().then(ports => {
      sendPorts(ports);
    });
  }, 5000); // Actualiza la lista cada 5 segundos

  req.on('close', () => {
    clearInterval(intervalId);
    res.end();
  });
};
export default streamSerialPorts;