open Api;
open Types;

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
               RequestError("An error occured! Please, try again."),
             ),
           )
           |> resolve
         }
       )
  );
};

let maxItems = 6;
let transformSearchResult = ({total_results, items}: searchResponse) => {
  total_results,
  items: items->Belt.Array.shuffle->Belt.Array.slice(0, maxItems),
};

let searchAdvice = (query, dispatch) => {
  dispatch(SearchRequest(RequestLoading));
  Js.Promise.(
    searchAdvice(query)
    |> then_(result =>
         switch (result) {
         | Belt.Result.Ok(response) =>
           dispatch(
             SearchRequest(
               RequestSuccess(response |> transformSearchResult),
             ),
           )
           |> resolve
         | Belt.Result.Error () =>
           dispatch(
             SearchRequest(
               RequestError("An error occured! Please, try again."),
             ),
           )
           |> resolve
         }
       )
  );
};

[@react.component]
let make = () => {
  let (state, dispatch) = React.useReducer(reducer, initialState);

  React.useEffect0(() => {
    fetchRandom(dispatch) |> ignore;
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
           | NotAsked =>
             <Message type_=Information text="You haven't searched yet!" />
           | Loading(None) => <Spinner show=true />
           | (Success(data) | Loading(Some(data))) as searchState =>
             <>
               <Spinner show={RemoteData.isLoading(searchState)} />
               <SearchResult data />
             </>
           | Failure(err) => <Message type_=Error text=err />
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
          {ReasonReact.string("Random wisdom:")}
        </h2>
        {switch (state.randomAdvice) {
         | NotAsked => ReasonReact.null
         | Success(data) => <RandomAdvice data />
         | Loading(result) =>
           <>
             <div className="loader" />
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
