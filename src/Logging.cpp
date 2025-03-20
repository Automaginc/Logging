#include "Logging.hpp"

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <fstream>
#include <iostream>
#include <istream>
#include <regex>
#include <string>

#ifndef MACOS
#include <print>
#include <stacktrace>
#endif

#include <rang.hpp>

#ifndef MACOS
#define GRABLOGFILE(x)                                                                                                                                                                                                          \
    {                                                                                                                                                                                                                           \
        if (!Automaginc::Logging::log_file_initalised)                                                                                                                                                                          \
        {                                                                                                                                                                                                                       \
            InitLog();                                                                                                                                                                                                          \
        }                                                                                                                                                                                                                       \
                                                                                                                                                                                                                                \
        std::ofstream file(Automaginc::Logging::log_file, std::ios_base::app);                                                                                                                                                  \
        if (!file.is_open())                                                                                                                                                                                                    \
        {                                                                                                                                                                                                                       \
            Automaginc::Logging::Error::LogError(Automaginc::Logging::Error::GenerateError("Failed to open \"" + log_file.string() + "\"", "Either failed to open \"" + log_file.string() + "\", or failed to create"), false); \
        }                                                                                                                                                                                                                       \
                                                                                                                                                                                                                                \
        x;                                                                                                                                                                                                                      \
                                                                                                                                                                                                                                \
        file.close();                                                                                                                                                                                                           \
    }
#else
// Yea we don't write to the log on MacOS for the time being because of the print include not being usable
#define GRABLOGFILE(x)
#endif

void Automaginc::Logging::Error::LogError(Error error)
{
    if (static_cast<int>(error.requires_verbose) <= static_cast<int>(verbose))
    {
        std::string str;

        std::cerr << rang::fg::blue << error.time << rang::fg::red << ": " << error.title << ":" << std::endl
                  << error.description << std::endl
                  << "Stacktrace:" << std::endl
                  << error.stacktrace << std::endl
                  << std::endl
                  << rang::fg::reset;

        GRABLOGFILE(std::println(file, "{}: {}:\n{}\nStacktrace:\n{}\n", error.time, error.title, error.description, error.stacktrace));
    }
}

void Automaginc::Logging::Error::LogError(Error error, bool write_to_log)
{
    std::cerr << "Something has gone wrong with the log because this has been called...\n"
              << error.time << ": " << error.title << ":" << std::endl
              << error.description << std::endl
              << "Stacktrace:" << std::endl
              << error.stacktrace << std::endl
              << std::endl;

    if (write_to_log)
    {
        std::cerr << "Log could be safe - this overload shouldn't be called unless there's something wrong with the log though\n\n";
        GRABLOGFILE(std::println(file, "{}: {}:\n{}\nStacktrace:\n{}\n", error.time, error.title, error.description, error.stacktrace));
    }
}

void Automaginc::Logging::Error::Explode()
{
    // std::cerr << rang::fg::magenta << "Program requested explode. Listing log" << std::endl
    //           << rang::fg::reset;
    std::cerr << rang::fg::magenta << "Program requested explode." << std::endl
              << rang::fg::reset;
    // GRABLOGFILE(std::println(file, "Program requested explode. Listing log"));
    GRABLOGFILE(std::println(file, "Program requested explode."));
    /*
    unsigned int repeat_times = 0;
    for (size_t i = 0; i < Automaginc::Logging::log.size(); i++)
    {
        if (i != 0 && static_cast<bool>((repeat_times += static_cast<int>(Automaginc::Logging::log[i].IsSame(Automaginc::Logging::log[i - 1])))))
        {
            continue;
        }

        if (repeat_times != 0)
        {
            std::cerr << rang::fg::magenta << repeat_times + 1 << rang::fg::reset << "\n";
            GRABLOGFILE(std::println("{}", repeat_times + 1));
            repeat_times = 0;
        }

        std::cerr << rang::fg::magenta << i << ":" << rang::fg::reset << " ";
        GRABLOGFILE(std::print(file, "{}: ", i));
        Automaginc::Logging::log[i].Print();
    }
    */

    // std::cerr << rang::fg::magenta << "Done listing log. std::abort-ing" << rang::fg::reset;
    // GRABLOGFILE(std::println(file, "Done listing log. std::abort-ing"));
    std::cerr << rang::fg::magenta << "std::abort-ing" << std::endl
              << rang::fg::reset;
    GRABLOGFILE(std::println(file, "std::abort-ing"));

    // it's safe to call abort here - because we already said the stacktrace
    std::abort();
}

auto Automaginc::Logging::Error::GenerateError(std::string title, std::string description, bool requires_verbose) -> Automaginc::Logging::Error
{
    Error error = Error();
    error.title = title;
    error.description = description;

#ifndef MACOS
    auto stacktrace = std::stacktrace::current();
    error.stacktrace = std::to_string(stacktrace);
#else
    error.stacktrace = "Stacktraces aren't supported on MacOS!\n";
#endif

    error.time = Automaginc::Logging::GetFormattedTime();
    error.requires_verbose = requires_verbose;

    return error;
}

void Automaginc::Logging::Error::CreateError(std::string title, std::string description, bool requires_verbose)
{
    Error error = GenerateError(title, description, requires_verbose);
    Automaginc::Logging::log.push_back(EWL::GenerateError(error));
    LogError(error);
}

void Automaginc::Logging::Error::CreateErrorProgramPause(std::string title, std::string description, bool requires_verbose)
{
    Error error = GenerateError(title, description, requires_verbose);
    Automaginc::Logging::log.push_back(EWL::GenerateError(error));
    LogError(error);

    std::cout << "Press Enter to continue...\n";
    GRABLOGFILE(std::print(file, "Press Enter to continue..."));
    std::cin.get();
}

void Automaginc::Logging::Error::CreateErrorProgramExplode(std::string title, std::string description, bool requires_verbose)
{
    Error error = GenerateError(title, description, requires_verbose);
    Automaginc::Logging::log.push_back(EWL::GenerateError(error));
    LogError(error);

    Explode();
}

auto Automaginc::Logging::Log::GenerateLog(std::string title, std::string description, bool requires_verbose) -> Automaginc::Logging::Log
{
    Log thislog;
    thislog.title = title;
    thislog.description = description;
    thislog.time = Automaginc::Logging::GetFormattedTime();
    thislog.requires_verbose = requires_verbose;

    return thislog;
}

void Automaginc::Logging::Log::LogLog(Log thislog)
{
    if (static_cast<int>(thislog.requires_verbose) <= static_cast<int>(verbose))
    {
        std::cout << rang::fg::blue << thislog.time << rang::fg::green << ": " << thislog.title << ":\n"
                  << thislog.description << "\n\n"
                  << rang::fg::reset;
        GRABLOGFILE(std::println(file, "{}: {}:\n{}\n", thislog.time, thislog.title, thislog.description));
    }
}

void Automaginc::Logging::Log::CreateLog(std::string title, std::string description, bool requires_verbose)
{
    Log thislog = GenerateLog(title, description, requires_verbose);
    log.push_back(EWL::GenerateLog(thislog));
    LogLog(thislog);
}

auto Automaginc::Logging::GetFormattedTime() -> std::string
{
    std::time_t time = std::time(nullptr);   // get time now
    // NOLINTNEXTLINE
    std::tm now = *std::unique_ptr<std::tm>(std::localtime(&time));
    const int fix_time = 1900;
    return "[" + std::to_string(now.tm_mday) + "/" + std::to_string(now.tm_mon) + "/" + std::to_string(now.tm_year + fix_time) + " " + std::to_string(now.tm_hour) + ":" + std::to_string(now.tm_min) + ":" + std::to_string(now.tm_sec) + "]";
}

void Automaginc::Logging::InitLog()
{
#ifndef MACOS
    std::string log_file_name = log_file_beginning + std::regex_replace(GetFormattedTime(), std::regex(":"), ";") + ".txt";
    log_file = log_file_location / log_file_name;
    std::filesystem::create_directories(log_file_location);

    std::ofstream file(Automaginc::Logging::log_file);
    if (!file.is_open())
    {
        // we call the longer method to avoid trying to write to the log again
        Automaginc::Logging::Error::LogError(Automaginc::Logging::Error::GenerateError("Failed to open \"" + log_file.string() + "\"", "Either failed to open \"" + log_file.string() + "\", or failed to create"), false);
    }

    std::println("{}: Log Initalized!\n", GetFormattedTime());
    std::println(file, "{}: Log Initalized!\n", GetFormattedTime());

    file.close();

    log_file_initalised = true;
#else
    std::cout << "Log files aren't supported on MacOS!" << std::endl;
#endif
}

auto Automaginc::Logging::EWL::GenerateLog(Log log) -> Automaginc::Logging::EWL
{
    EWL elboth;
    elboth.log = log;
    return elboth;
}

auto Automaginc::Logging::EWL::GenerateWarning(Warning warning) -> Automaginc::Logging::EWL
{
    EWL elboth;
    elboth.warning = warning;
    return elboth;
}

auto Automaginc::Logging::EWL::GenerateError(Error error) -> Automaginc::Logging::EWL
{
    EWL elboth;
    elboth.error = error;
    return elboth;
}

void Automaginc::Logging::EWL::Print()
{
    if (error.has_value())
    {
        Error::LogError(error.value());
    } else if (log.has_value())
    {
        Log::LogLog(log.value());
    } else if (warning.has_value())
    {
        Warning::LogWarning(warning.value());
    } else
    {
        Error::LogError(Error::GenerateError("I don't contain error or log or warning?", "Wait, I don't contain a log or an error or a warning?"));
    }
}

auto Automaginc::Logging::Warning::GenerateWarning(std::string title, std::string description, bool requires_verbose, bool use_stacktrace,
                                                   bool use_stacktrace_verbose) -> Automaginc::Logging::Warning
{
    Warning warning;
    warning.title = title;
    warning.description = description;
    warning.time = Automaginc::Logging::GetFormattedTime();
    warning.requires_verbose = requires_verbose;
    if (use_stacktrace || (use_stacktrace_verbose && verbose))
    {
#ifndef MACOS
        auto stacktrace = std::stacktrace::current();
        warning.stacktrace = std::to_string(stacktrace);
#else
        warning.stacktrace = "Stacktraces aren't supported on MacOS!\n";
#endif
    }

    return warning;
}

void Automaginc::Logging::Warning::CreateWarning(std::string title, std::string description, bool requires_verbose, bool use_stacktrace, bool use_stacktrace_verbose)
{
    Warning warning = GenerateWarning(title, description, requires_verbose, use_stacktrace, use_stacktrace_verbose);
    log.push_back(EWL::GenerateWarning(warning));
    LogWarning(warning);
}

void Automaginc::Logging::Warning::LogWarning(Warning thiswarning)
{
    if (static_cast<int>(thiswarning.requires_verbose) <= static_cast<int>(verbose))
    {
        std::cout << rang::fg::blue << thiswarning.time << rang::fg::yellow << ": " << thiswarning.title << ":\n"
                  << thiswarning.description << "\n";
        GRABLOGFILE(std::println(file, "{}: {}:\n{}", thiswarning.time, thiswarning.title, thiswarning.description));
        if (thiswarning.use_stacktrace || (thiswarning.use_stacktrace_verbose && verbose))
        {
            std::cout << "Stacktrace:\n"
                      << thiswarning.stacktrace << "\n\n"
                      << rang::fg::reset;
            GRABLOGFILE(std::println(file, "Stacktrace:{}", thiswarning.stacktrace));
        } else
        {
            std::cout << "\n"
                      << rang::fg::reset;
        }
    }
}

auto Automaginc::Logging::Error::IsSameAs(Error thiserror) const -> bool
{
    return title == thiserror.title && description == thiserror.description;
}

auto Automaginc::Logging::Warning::IsSameAs(Warning thiswarning) const -> bool
{
    return title == thiswarning.title && description == thiswarning.description;
}

auto Automaginc::Logging::Log::IsSameAs(Log thislog) const -> bool
{
    return title == thislog.title && description == thislog.description;
}

auto Automaginc::Logging::EWL::IsSame(Automaginc::Logging::EWL thisewl) -> bool
{
    if (thisewl.log.has_value() && this->log.has_value() && thisewl.log.value().IsSameAs(this->log.value()))
    {
        return true;
    }
    if (thisewl.warning.has_value() && this->warning.has_value() && thisewl.warning.value().IsSameAs(this->warning.value()))
    {
        return true;
    }
    if (thisewl.error.has_value() && this->error.has_value() && thisewl.error.value().IsSameAs(this->error.value()))
    {
        return true;
    }

    return false;
}

auto Automaginc::Logging::SetupTerminator() -> bool
{
    std::set_terminate([]() {
        Error::CreateError("UNHANDLED EXCEPTION FROM std::terminate", "An Unhandled Exception was called from std::terminate - attemping to get message", false);
        try
        {
            std::rethrow_exception(std::current_exception());
        } catch (const std::exception& std_exception) // basically hope and pray someone's not a idiot and uses non std::exception based classes for their throw, and hopefully if they do it's on of these types
        {
            Warning::CreateWarning("FOUND UNHANDLED EXCEPTION WHAT() AS std::exception", std_exception.what(), false, false, true);
        } catch (const std::string& str_exception)
        {
            Warning::CreateWarning("FOUND UNHANDLED EXCEPTION WHAT() AS std::string", str_exception, false, false, true);
        } catch (const int& int_exception)
        {
            Warning::CreateWarning("FOUND UNHANDLED EXCEPTION WHAT() AS int", std::to_string(int_exception), false, false, true);
        } catch (const char*& char_ptr_exception)
        {
            Warning::CreateWarning("FOUND UNHANDLED EXCEPTION WHAT() AS const char*", char_ptr_exception, false, false, true);
        } catch (const char& char_exception) // an actual char
        {
            Warning::CreateWarning("FOUND UNHANDLED EXCEPTION WHAT() AS char (no *)", std::to_string(char_exception), false, false, true);
        } catch (const unsigned int& unsigned_int_exception)
        {
            Warning::CreateWarning("FOUND UNHANDLED EXCEPTION WHAT() AS int", std::to_string(unsigned_int_exception), false, false, true);
        } catch (...)
        {
            Warning::CreateWarning("COULDN'T FIND UNHANDLED EXCEPTION WHAT()", "Simply couldn't find out what type it is. Not a defined type in Automaginc::Logging::SetupTerminator", false, false, true);
        }
        std::abort();
    });

    if (setup_terminator_private)
    {
        Warning::CreateWarning("std::terminate setted up again", "Successfully setup std::terminate setup, again?");
    } else
    {
        Log::CreateLog("std::terminate setted up", "Successfully setup std::terminator!");
    }

    return true;
}
