[@react.component]
let make = (~show=false) => {
  <div className="spinner-container">
    {show ? <div className="spinner" /> : ReasonReact.null}
  </div>;
};
