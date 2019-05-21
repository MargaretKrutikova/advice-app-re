type advice = {
  id: string,
  value: string,
};

type searchResponse = {
  total_results: int,
  items: array(advice),
};

type randomResponse = {item: advice};

type message = {type_: string};

type messageType =
  | Information
  | Error;
