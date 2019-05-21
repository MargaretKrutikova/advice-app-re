open Types;

[@react.component]
let make = (~data: searchResponse) => {
  switch (data.items) {
  | [||] => <Message type_=Information text="Ooops... No advice found!" />
  | items =>
    <ol className="search-result-list">
      {items
       |> Array.map(item =>
            <li key={item.id} className="search-result-item">
              {ReasonReact.string(item.value)}
            </li>
          )
       |> ReasonReact.array}
    </ol>
  };
};
