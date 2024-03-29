import { SerialPort } from "serialport";
export const renderIndex = (req, res) => {
  SerialPort.list()
  .then(ports => {
    res.render("index", { ports });
  })
  .catch(err => {
    console.error("Error obteniendo la lista de puertos seriales:", err);
    res.status(500).send("Error obteniendo la lista de puertos seriales");
  });
};

export const renderAbout = (req, res) => {
  res.render("about");
};
