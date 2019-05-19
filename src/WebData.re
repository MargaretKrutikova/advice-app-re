type t('a) = RemoteData.t('a, option('a), string);

type apiAction('a) =
  | RequestLoading
  | RequestError(string)
  | RequestSuccess('a);

let toLoading = (data: t('a)): t('a) =>
  RemoteData.(Loading(data |> map(d => Some(d)) |> withDefault(None)));

let updateWebData = (data: t('a), action: apiAction('a)): t('a) => {
  switch (action) {
  | RequestLoading => data |> toLoading
  | RequestError(error) => RemoteData.Failure(error)
  | RequestSuccess(response) => RemoteData.Success(response)
  };
};
