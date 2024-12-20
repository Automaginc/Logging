#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace Automaginc
{
struct Logging
{
    struct Log
    {
        static auto GenerateLog(std::string title, std::string description, bool requires_verbose = true) -> Log;
        static void LogLog(Log thislog);
        static void CreateLog(std::string title, std::string description, bool requires_verbose = true);

        std::string title;
        std::string description;
        std::string time;
        bool requires_verbose{};

        [[nodiscard]] auto IsSameAs(Log thislog) const -> bool; // not a robust function - should only be used internally
    };

    struct Warning
    {
        static auto GenerateWarning(std::string title, std::string description, bool requires_verbose = true, bool use_stacktrace = false, bool use_stacktrace_verbose = false) -> Warning;
        static void LogWarning(Warning thiswarning);
        static void CreateWarning(std::string title, std::string description, bool requires_verbose = true, bool use_stacktrace = false, bool use_stacktrace_verbose = false);

        std::string title;
        std::string description;
        std::string time;
        std::string stacktrace;
        bool use_stacktrace{};
        bool use_stacktrace_verbose{};
        bool requires_verbose{};

        [[nodiscard]] auto IsSameAs(Warning thiswarning) const -> bool; // not a robust function - should only be used internally
    };

    struct Error
    {
        static void LogError(Error error);
        static void Explode();
        static auto GenerateError(std::string title, std::string description, bool requires_verbose = false) -> Error;
        static void CreateError(std::string title, std::string description, bool requires_verbose = false);
        static void CreateErrorProgramPause(std::string title, std::string description, bool requires_verbose = false);
        static void CreateErrorProgramExplode(std::string title, std::string description, bool requires_verbose = false);

        std::string title;
        std::string description;
        std::string stacktrace;
        std::string time;
        bool requires_verbose{};

        friend Log;
        friend Warning;
        friend Logging;

        [[nodiscard]] auto IsSameAs(Error error) const -> bool;

      private:
        static void
        LogError(Error error, bool write_to_log); // stripped down - only for emergencies, mainly disk write failure
    };

    struct EWL
    {
        static auto GenerateLog(Log log) -> EWL;
        static auto GenerateWarning(Warning warning) -> EWL;
        static auto GenerateError(Error error) -> EWL;

        void Print();

        std::optional<Log> log;
        std::optional<Warning> warning;
        std::optional<Error> error;

        auto IsSame(EWL thisewl) -> bool;
    };

    static void SetupTerminator();

    static auto GetFormattedTime() -> std::string;
    static void InitLog();

    inline static std::vector<EWL> log;

#ifdef DEBUG
    inline static bool verbose = true;
    inline static bool write_to_file = true;
#else
    inline static bool verbose = false;
    inline static bool write_to_file = false;
#endif
    inline static std::filesystem::path log_file_location = "Logs";
    inline static std::string log_file_beginning = "Automaginc";
    inline static std::filesystem::path log_file;

  private:
    inline static bool log_file_initalised = false;
    inline static bool setup_terminator_private = SetupTerminator();
};
} // namespace Automaginc
