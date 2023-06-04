namespace Utilities
{
    public static class Result
    {
        public static Success<T> Success<T>(T value) => new Success<T>(value);

        public static Failure<T> Failure<T>(string error) => new Failure<T>(error);
    }

    public class Result<T>
    {
        public bool IsError { get; protected set; }
        public T? Value { get; protected set; }
        public string? Error { get; protected set; }
    }

    public class Success<T> : Result<T>
    {
        public Success(T value)
        {
            IsError = false;
            Value = value;
            Error = null;
        }
    }

    public class Failure<T> : Result<T>
    {
        public Failure(string error)
        {
            IsError = true;
            Value = default;
            Error = error;
        }
    }
}