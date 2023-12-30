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

    client.current.on("message", function (topic, message) {
      console.log(topic);
      console.log(message);
      if (topic === "event/delete/success") navigate("/calendar");
    });
  }, []);

  const handleDetails = () => {
    client.current.publish(
      "event/delete",
      JSON.stringify({ id: state.id }),
      (error) => console.log(error)
    );
  };

  return (
    <div>
      <div className="w-[100%] flex items-center justify-end pr-[20px] pt-[10px]">
        <div className="w-[30%] flex items-center gap-[20px]">
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
          {Object.entries(state).map(
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
