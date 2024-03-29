import { Router } from "express";
import { renderpageForm, renderpageadminForm, renderREAForm, } from "../controllers/pages.controller.js";

const router = Router();

// cargar paginas
router.get("/", renderpageForm);
router.get("/pages/recursosadmin", renderpageadminForm );
router.get("/pages/FormularioREA", renderREAForm );
//_________________________________________________________________________________________

export default router;