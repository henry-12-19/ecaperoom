
import path from "path";
import fs from "fs-extra";
import { SerialPort } from "serialport";

export const renderpageForm  = async (req, res) => {
res.render("sidebar/sidebar");
  };
export const renderREAForm = (req, res) => res.render("sidebar/sidebar");

export const renderpageadminForm = async (req, res) => {
  
  res.render("sidebar/sidebar");
};


