import axios from "axios";

const API = axios.create({ baseURL: "http://127.0.0.1:1880" });

export const getEvents = () => API.get("/events");
export const getCyclicEvents = () => API.get("/cyclic_events");
export const getProposeEvents = () => API.get("/proposed_event");
