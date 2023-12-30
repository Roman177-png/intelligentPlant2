import React from "react";
import { Routes, Route, Navigate, BrowserRouter } from "react-router-dom";
import { Calendar, Event, Navbar } from "views";

const Root = () => {
  return (
    <BrowserRouter>
      <Navbar />
      <Routes>
        <Route path="/calendar" element={<Calendar />} />
        <Route path="/event" element={<Event />} />
        <Route path="*" element={<Navigate to="/calendar" />} />
      </Routes>
    </BrowserRouter>
  );
};

export default Root;
