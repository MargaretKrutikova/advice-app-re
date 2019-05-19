open Api;

type remoteSearchResult = WebData.t(searchResponse);
type remoteRandomAdvice = WebData.t(advice);

type state = {
  searchQuery: string,
  searchResult: remoteSearchResult,
  randomAdvice: remoteRandomAdvice,
};

type action =
  | SearchQueryChange(string)
  | SearchRequest(WebData.apiAction(searchResponse))
  | RandomAdviceRequest(WebData.apiAction(advice));

let reducer = (state, action) => {
  switch (action) {
  | SearchQueryChange(searchQuery) => {...state, searchQuery}
  | SearchRequest(searchAction) => {
      ...state,
      searchResult: WebData.updateWebData(state.searchResult, searchAction),
    }
  | RandomAdviceRequest(randomAction) => {
      ...state,
      randomAdvice: WebData.updateWebData(state.randomAdvice, randomAction),
    }
  };
};

let initialState = {
  searchQuery: "",
  searchResult: RemoteData.NotAsked,
  randomAdvice: RemoteData.NotAsked,
};

let fetchRandom = dispatch => {
  dispatch(RandomAdviceRequest(RequestLoading));
  Js.Promise.(
    fetchRandomAdvice()
    |> then_(result =>
         switch (result) {
         | Belt.Result.Ok(response) =>
           dispatch(RandomAdviceRequest(RequestSuccess(response.item)))
           |> resolve
         | Belt.Result.Error () =>
           dispatch(
             RandomAdviceRequest(
               RequestError(
                 "Sorry, something bad happened! Please, try again.",
               ),
             ),
           )
           |> resolve
         }
       )
  );
};

let searchAdvice = (query, dispatch) => {
  dispatch(SearchRequest(RequestLoading));
  Js.Promise.(
    searchAdvice(query)
    |> then_(result =>
         switch (result) {
         | Belt.Result.Ok(response) =>
           dispatch(SearchRequest(RequestSuccess(response))) |> resolve
         | Belt.Result.Error () =>
           dispatch(
             SearchRequest(
               RequestError(
                 "Sorry, something bad happened! Please, try again.",
               ),
             ),
           )
           |> resolve
         }
       )
  );
};

module SearchResult = {
  [@react.component]
  let make = (~data: searchResponse) => {
    switch (data.items) {
    | [||] => ReasonReact.string("Ooops... No advice found!")
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
};

module RandomAdvice = {
  [@react.component]
  let make = (~data: advice) => {
    <div className="random-advice-container">
      <div className="random-advice"> {ReasonReact.string(data.value)} </div>
    </div>;
  };
};

[@react.component]
let make = () => {
  let (state, dispatch) = React.useReducer(reducer, initialState);

  React.useEffect0(() => {
    searchAdvice("hair", dispatch) |> ignore;
    None;
  });

  let handleSearchChange = event => {
    let searchQuery = ReactEvent.Form.target(event)##value;
    dispatch(SearchQueryChange(searchQuery));
  };

  let handleKeyDown = event => {
    let keyCode = ReactEvent.Keyboard.which(event);
    if (keyCode == 13) {
      searchAdvice(state.searchQuery, dispatch) |> ignore;
    };
  };

  let handleRandomClick = _ => {
    fetchRandom(dispatch) |> ignore;
  };

  <>
    <div className="background" />
    <div className="content">
      <div className="panel">
        <h2 className="title"> {ReasonReact.string("Ask for advice")} </h2>
        <div>
          <input
            placeholder="Search..."
            className="text-input"
            value={state.searchQuery}
            onChange=handleSearchChange
            onKeyDown=handleKeyDown
            disabled={RemoteData.isLoading(state.searchResult)}
          />
          {switch (state.searchResult) {
           | NotAsked => ReasonReact.null
           | Success(data) => <SearchResult data />
           | Loading(result) =>
             <>
               <div> {ReasonReact.string("Loading...")} </div>
               {result
                ->Belt.Option.flatMap(data => Some(<SearchResult data />))
                ->Belt.Option.getWithDefault(ReasonReact.null)}
             </>
           | Failure(err) => <div> {ReasonReact.string(err)} </div>
           }}
        </div>
      </div>
      <div className="delimiter" />
      <div className="panel">
        <button
          onClick=handleRandomClick
          disabled={RemoteData.isLoading(state.randomAdvice)}>
          {ReasonReact.string("Refresh")}
        </button>
        <h2 className="title title--small">
          {ReasonReact.string("Wisdom of the day:")}
        </h2>
        {switch (state.randomAdvice) {
         | NotAsked => ReasonReact.null
         | Success(data) => <RandomAdvice data />
         | Loading(result) =>
           <>
             <div> {ReasonReact.string("Loading...")} </div>
             {result
              ->Belt.Option.flatMap(data => Some(<RandomAdvice data />))
              ->Belt.Option.getWithDefault(ReasonReact.null)}
           </>
         | Failure(err) => <div> {ReasonReact.string(err)} </div>
         }}
      </div>
    </div>
  </>;
};
