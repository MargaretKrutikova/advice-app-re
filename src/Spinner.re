[@react.component]
let make = (~show=false) => {
  show ? <div className="spinner" /> : ReasonReact.null;
};
