import React, { useEffect, useRef } from "react";
import { NavLink, useLocation, useNavigate } from "react-router-dom";
import mqtt from "mqtt/dist/mqtt";

const Event = () => {
  const { state } = useLocation();
  const navigate = useNavigate();
  const propsFormat = { minimumIntegerDigits: 2, useGrouping: false };
  const client = useRef();

  const options = {
    protocol: "mqtts",
    reconnect: true,
  };

  useEffect(() => {
    client.current = mqtt.connect("mqtt://test.mosquitto.org:8081", options);
    client.current.subscribe("event/delete");
    client.current.subscribe("event/delete/success");
    client.current.subscribe("event/addEventsPlants");
    client.current.subscribe("event/addEventsPlants/success");

    client.current.on("message", function (topic, message) {
      console.log(topic);
      console.log(message);
      if (topic === "event/delete/success") navigate("/calendar");
      if (topic === "event/addEventsPlants/success") {
        //navigate("/calendar");
        console.log("TAK");
      }
    });
  }, []);

  const handleDetails = () => {
    client.current.publish(
      "event/delete",
      JSON.stringify({ id: state.event.id }),
      (error) => console.log(error)
    );
  };

  console.log(state.eventsData);

  const handleEventNextMonth = () => {
    const { start, end, title, description, created } = state.event;
    console.log(end.getMonth());
    const startEvent = new Date(
      start.getMonth() + 1 == 12
        ? start.getFullYear() + 1
        : start.getFullYear(),
      start.getMonth() + 1 == 12 ? 0 : start.getMonth() + 1,
      start.getDate(),
      start.getHours(),
      start.getMinutes(),
      start.getSeconds()
    );
    const endEvent = new Date(
      end.getMonth() + 1 == 12 ? end.getFullYear() + 1 : end.getFullYear(),
      end.getMonth() + 1 == 12 ? 0 : end.getMonth() + 1,
      end.getDate(),
      end.getHours(),
      end.getMinutes(),
      end.getSeconds()
    );
    const addEvent = {
      date: startEvent,
      start: startEvent,
      end: endEvent,
      title: title,
      description: description,
    };
    const findEvent = state.eventsData.find((events) => {
      return (
        addEvent.start.toLocaleString() == events.start.toLocaleString() &&
        addEvent.title == events.title &&
        addEvent.description == events.description
      );
    });

    console.log(findEvent);
    if (!findEvent) {
      client.current.publish(
        "event/addEventsPlants",
        JSON.stringify({ addEvent }),
        (error) => console.log(error)
      );
      navigate("/calendar");
    } else {
      console.log("Wydarzenie istnieje");
    }
  };
  const handleEventsNextMonth = () => {
    const currentMonth = state.event.start.getMonth();

    // Filtruj wydarzenia, aby znaleźć te zaplanowane w tym samym miesiącu
    const eventsInSameMonth = state.eventsData.filter(
      (event) =>
        currentMonth == event.start.getMonth() && event.title == "Podlewanie"
    );
    console.log(eventsInSameMonth);
    eventsInSameMonth.forEach((event) => {
      const { start, end, title, description, created } = event;
      console.log(end.getMonth());
      const startEvent = new Date(
        start.getMonth() + 1 == 12
          ? start.getFullYear() + 1
          : start.getFullYear(),
        start.getMonth() + 1 == 12 ? 0 : start.getMonth() + 1,
        start.getDate(),
        start.getHours(),
        start.getMinutes(),
        start.getSeconds()
      );
      const endEvent = new Date(
        end.getMonth() + 1 == 12 ? end.getFullYear() + 1 : end.getFullYear(),
        end.getMonth() + 1 == 12 ? 0 : end.getMonth() + 1,
        end.getDate(),
        end.getHours(),
        end.getMinutes(),
        end.getSeconds()
      );
      const addEvent = {
        date: startEvent,
        start: startEvent,
        end: endEvent,
        title: title,
        description: description,
      };
      const findEvent = state.eventsData.find((events) => {
        return (
          addEvent.start.toLocaleString() == events.start.toLocaleString() &&
          addEvent.title == events.title &&
          addEvent.description == events.description
        );
      });
      if (!findEvent) {
        client.current.publish(
          "event/addEventsPlants",
          JSON.stringify({ addEvent }),
          (error) => console.log(error)
        );
      } else {
        console.log("Wydarzenie istnieje");
      }
    });
    navigate("/calendar");
  };

  return (
    <div>
      <div className="w-[100%] flex items-center justify-end pr-[20px] pt-[10px]">
        <div className="flex items-center gap-[20px]">
          {state.event.title == "Podlewanie" && (
            <div
              className="flex items-center border-0 box-border shadow-none m-0 py-2 px-5 bg-[#3174ad] rounded-md text-white w-[50%] text-left  cursor-pointer"
              onClick={handleEventNextMonth}
            >
              Ustaw podlewanie na ten sam dzień w następnym miesiacu
            </div>
          )}
          {state.event.title == "Podlewanie" && (
            <div
              className="flex items-center border-0 box-border shadow-none m-0 py-2 px-5 bg-[#3174ad] rounded-md text-white w-[50%] text-left  cursor-pointer"
              onClick={handleEventsNextMonth}
            >
              Ustaw podlewania zaplanowane w tym miesiącu na następnym
            </div>
          )}
          <NavLink
            to="http://127.0.0.1:1880/ui/#!/2?socketid=aV6mkA2jXZNhKTvqAAAB"
            className="flex items-center border-0 box-border shadow-none m-0 py-2 px-5 bg-[#3174ad] rounded-md text-white w-[50%] text-left capitalize cursor-pointer"
          >
            Edit
          </NavLink>
          <div
            className="flex items-center border-0 box-border shadow-none m-0 py-2 px-5 bg-[red] rounded-md text-white w-[50%] text-left capitalize cursor-pointer"
            onClick={handleDetails}
          >
            Delete
          </div>
        </div>
      </div>
      <div className="flex justify-center h-[90vh] items-center bg-[white]">
        <div className="flex flex-col w-[70vw] max-h-[80vh] px-[20px] py-[10px] rounded-[4px] border-[3px] shadow-2xl border-[#ccc] bg-[white] overflow-y-auto leading-normal">
          <h1 className="text-center mb-4 mt-4 text-3xl font-medium leading-tight text-[black] ">
            Details information about event
          </h1>
          {Object.entries(state.event).map(
            ([key, value]) =>
              value != "undefined" && (
                <div className="flex flex-rows gap-3 mb-2" key={key}>
                  <div className="flex items-center border-0 box-border shadow-none m-0 py-2 px-5 bg-[#3174ad] rounded-md text-white w-[50%] text-left capitalize">
                    {key}
                  </div>
                  {key === "start" || key === "end" ? (
                    <div className="w-[80%] inline-block m-0 text-center align-middle bg-transparent border border-gray-300 py-1 px-4 leading-normal">
                      {new Date(value)
                        .getDate()
                        .toLocaleString("en-US", propsFormat)}
                      .
                      {(
                        Number(
                          new Date(value)
                            .getMonth()
                            .toLocaleString("en-US", propsFormat)
                        ) + 1
                      ).toLocaleString("en-US", propsFormat)}
                      .{new Date(value).getFullYear()}{" "}
                      {new Date(value)
                        .getHours()
                        .toLocaleString("en-US", propsFormat)}
                      :
                      {new Date(value)
                        .getMinutes()
                        .toLocaleString("en-US", propsFormat)}
                      :
                      {new Date(value)
                        .getSeconds()
                        .toLocaleString("en-US", propsFormat)}
                    </div>
                  ) : (
                    <div className="w-[80%] inline-block m-0 text-center align-middle bg-transparent border border-gray-300 py-1 px-4 leading-normal">
                      {key === "created"
                        ? new Date(value).toLocaleString("sv", {
                            timeZone: "Europe/Paris",
                          })
                        : value}
                    </div>
                  )}
                </div>
              )
          )}
        </div>
      </div>
    </div>
  );
};

export default Event;
