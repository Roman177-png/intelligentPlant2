import React, { useState, useEffect, useRef } from "react";
import { Calendar, momentLocalizer } from "react-big-calendar";
import moment from "moment";
import { useNavigate } from "react-router-dom";
import * as api from "api/index.js";
import "react-big-calendar/lib/css/react-big-calendar.css";
import mqtt from "mqtt/dist/mqtt";

const CalendarView = () => {
  const [eventsData, setEventsData] = useState();
  moment.locale("en-GB");
  const localizer = momentLocalizer(moment);
  const navigate = useNavigate();
  const client = useRef();
  const options = {
    protocol: "mqtts",
    reconnect: true,
  };

  useEffect(() => {
    client.current = mqtt.connect("mqtt://test.mosquitto.org:8081", options);
    const fetchAssets = async () => {
      try {
        const { data } = await api.getEvents();
        const event = data.map((props) => {
          const { id, date, start, end, type, description, created } = props;
          const date_start = new Date(String(date));

          const year_start = new Intl.DateTimeFormat("en", {
            year: "numeric",
          }).format(date_start);
          const month_start = new Intl.DateTimeFormat("en", {
            month: "numeric",
          }).format(date_start);
          const day_start = new Intl.DateTimeFormat("en", {
            day: "numeric",
          }).format(date_start);

          const next_date_start = date_start.setDate(date_start.getDate() + 1);

          const year_next_date_start = new Intl.DateTimeFormat("en", {
            year: "numeric",
          }).format(next_date_start);
          const month_next_date_start = new Intl.DateTimeFormat("en", {
            month: "numeric",
          }).format(next_date_start);
          const day_next_date_start = new Intl.DateTimeFormat("en", {
            day: "numeric",
          }).format(next_date_start);

          const year_end = start < end ? year_start : year_next_date_start;
          const month_end = start < end ? month_start : month_next_date_start;
          const day_end = start < end ? day_start : day_next_date_start;

          const startSplit = start.split(":");
          const endSplit = end.split(":");
          return {
            id: id,
            start: new Date(
              year_start,
              month_start - 1,
              day_start,
              startSplit[0],
              startSplit[1],
              startSplit[2]
            ),
            end: new Date(
              year_end,
              month_end - 1,
              day_end,
              endSplit[0],
              endSplit[1],
              endSplit[2]
            ),
            title: type,
            description: description,
            created: created,
          };
        });
        setEventsData(event);
      } catch {}
    };
    fetchAssets();
    client.current.subscribe("event/add/calendar");
    client.current.on("message", function (topic, message) {
      console.log("TAK");
      fetchAssets();
    });
  }, []);

  return (
    <>
      <Calendar
        views={["day", "agenda", "work_week", "month"]}
        selectable
        localizer={localizer}
        defaultDate={new Date()}
        defaultView="month"
        events={eventsData}
        style={{ height: "95vh" }}
        onSelectEvent={(event) => navigate("/event", { state: event })}
      />
    </>
  );
};
export default CalendarView;
