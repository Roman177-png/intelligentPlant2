import React, { useState, useEffect } from "react";
import { NavLink } from "react-router-dom";

const Nabar = () => {
  var [date, setDate] = useState(new Date());

  useEffect(() => {
    var timer = setInterval(() => setDate(new Date()), 1000);
    return function cleanup() {
      clearInterval(timer);
    };
  });

  return (
    <div className="flex h-[5vh] items-center border-0 box-border shadow-none m-0 py-2 px-5 bg-[#3174ad] text-white w-full text-left">
      <NavLink className="w-[60%]" to="/calendar">
        Current data and time: {date.toLocaleDateString()}{" "}
        {date.toLocaleTimeString()}
      </NavLink>
      <div className="w-[40%] flex justify-end flex-row gap-[20px] ">
        <NavLink
          className="hover:border-b-[1px] hover:border-b-[white] navabar-active"
          to="http://127.0.0.1:1880/ui/#!/1?socketid=aV6mkA2jXZNhKTvqAAAB"
        >
          Add events
        </NavLink>
        <NavLink
          className="hover:border-b-[1px] hover:border-b-[white] navabar-active"
          to="/calendar"
        >
          Calendar
        </NavLink>
      </div>
    </div>
  );
};

export default Nabar;
