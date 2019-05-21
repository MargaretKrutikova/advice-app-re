open Json.Decode;
open Types;

/** decoders */

let decodeAdvice = (json): advice => {
  {
    id: json |> field("slip_id", string),
    value: json |> field("advice", string),
  };
};

let decodeSearchResult = (json): searchResponse => {
  {
    total_results: json |> field("total_results", string) |> int_of_string,
    items: json |> field("slips", array(decodeAdvice)),
  };
};

let decodeRandomResponse = (json): randomResponse => {
  {item: json |> field("slip", decodeAdvice)};
};

let decodeMessage = (json): message => {
  {type_: json |> Json.Decode.at(["message", "type"], string)};
};

let messageToResponse =
    (message: message): Belt.Result.t(searchResponse, unit) => {
  switch (message.type_) {
  // received empty response
  | "notice" => Belt.Result.Ok({total_results: 0, items: [||]})
  | _ => Belt.Result.Error()
  };
};

/** api calls */

let apiBaseUrl = "https://api.adviceslip.com/advice";

let searchAdvice =
    (query): Js.Promise.t(Belt.Result.t(searchResponse, unit)) =>
  Js.Promise.(
    Axios.get(apiBaseUrl ++ "/search/" ++ query)
    |> then_(response =>
         switch (response##data |> decodeSearchResult) {
         | searchResult => Belt.Result.Ok(searchResult) |> resolve
         // exception while decoding, could be that the response is message
         | exception _ =>
           switch (response##data |> decodeMessage) {
           | message => messageToResponse(message) |> resolve
           | exception _ => Belt.Result.Error() |> resolve
           }
         }
       )
    |> catch(_ => Belt.Result.Error() |> resolve)
  );

let fetchRandomAdvice = (): Js.Promise.t(Belt.Result.t(randomResponse, unit)) =>
  Js.Promise.(
    Axios.get(apiBaseUrl)
    |> then_(response =>
         Belt.Result.Ok(response##data |> decodeRandomResponse) |> resolve
       )
    |> catch(_ => Belt.Result.Error() |> resolve)
  );
