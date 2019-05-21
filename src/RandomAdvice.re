open Types;

[@react.component]
let make = (~data: advice) => {
  <div className="random-advice-container">
    <div className="random-advice"> {ReasonReact.string(data.value)} </div>
  </div>;
};
