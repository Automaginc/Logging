#include "Logging.hpp"

#include <chrono>
#include <cstdlib>
#include <exception>
#include <format>
#include <fstream>
#include <iostream>
#include <print>
#include <regex>
#include <stacktrace>

#include <rang.hpp>
#include <string>

#define GRABLOGFILE(x)                                                                                                                                                                                                                      \
    {                                                                                                                                                                                                                                       \
        if (!AppleUIFramework::Logging::log_file_initalised)                                                                                                                                                                                \
        {                                                                                                                                                                                                                                   \
            InitLog();                                                                                                                                                                                                                      \
        }                                                                                                                                                                                                                                   \
                                                                                                                                                                                                                                            \
        std::ofstream file(AppleUIFramework::Logging::log_file, std::ios_base::app);                                                                                                                                                        \
        if (!file.is_open())                                                                                                                                                                                                                \
        {                                                                                                                                                                                                                                   \
            AppleUIFramework::Logging::Error::LogError(AppleUIFramework::Logging::Error::GenerateError("Failed to open \"" + log_file.string() + "\"", "Either failed to open \"" + log_file.string() + "\", or failed to create"), false); \
        }                                                                                                                                                                                                                                   \
                                                                                                                                                                                                                                            \
        x;                                                                                                                                                                                                                                  \
                                                                                                                                                                                                                                            \
        file.close();                                                                                                                                                                                                                       \
    }

void AppleUIFramework::Logging::Error::LogError(Error error)
{
    if (static_cast<int>(error.requires_verbose) <= static_cast<int>(verbose))
    {
        std::string str;

        std::cerr << rang::fg::blue << error.time << rang::fg::red << ": " << error.title << ":" << std::endl
                  << error.description << std::endl
                  << "Stacktrace:" << std::endl
                  << error.stacktrace << std::endl
                  << rang::fg::reset;

        GRABLOGFILE(std::println(file, "{}: {}:\n{}\nStacktrace:\n{}", error.time, error.title, error.description, error.stacktrace));
    }
}

void AppleUIFramework::Logging::Error::LogError(Error error, bool write_to_log)
{
    std::cerr << "Something has gone wrong with the log because this has been called...\n"
              << error.time << ": " << error.title << ":" << std::endl
              << error.description << std::endl
              << "Stacktrace:" << std::endl
              << error.stacktrace << std::endl;

    if (write_to_log)
    {
        std::cerr << "Log could be safe - this overload shouldn't be called unless there's something wrong with the log though\n";
        GRABLOGFILE(std::println(file, "{}: {}:\n{}\nStacktrace:\n{}", error.time, error.title, error.description, error.stacktrace));
    }
}

void AppleUIFramework::Logging::Error::Explode()
{
    std::cerr << rang::fg::magenta << "Program requested explode. Listing log" << std::endl
              << rang::fg::reset;

    GRABLOGFILE(std::println(file, "Program requested explode. Listing log"));

    unsigned int repeat_times = 0;
    for (size_t i = 0; i < AppleUIFramework::Logging::log.size(); i++)
    {
        if (i != 0 && static_cast<bool>((repeat_times += static_cast<int>(AppleUIFramework::Logging::log[i].IsSame(AppleUIFramework::Logging::log[i - 1])))))
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
        AppleUIFramework::Logging::log[i].Print();
    }

    auto stacktrace = std::stacktrace::current();

    std::cerr << rang::fg::magenta << "Done listing log. std::abort-ing" << rang::fg::reset;
    GRABLOGFILE(std::println(file, "Done listing log. std::abort-ing"));

    // it's safe to call abort here - because we already said the stacktrace
    std::abort();
}

auto AppleUIFramework::Logging::Error::GenerateError(std::string title, std::string description, bool requires_verbose) -> AppleUIFramework::Logging::Error
{
    Error error = Error();
    error.title = title;
    error.description = description;
    auto stacktrace = std::stacktrace::current();
    error.stacktrace = std::to_string(stacktrace);
    error.time = AppleUIFramework::Logging::GetFormattedTime();
    error.requires_verbose = requires_verbose;

    return error;
}

void AppleUIFramework::Logging::Error::CreateError(std::string title, std::string description, bool requires_verbose)
{
    Error error = GenerateError(title, description, requires_verbose);
    AppleUIFramework::Logging::log.push_back(EWL::GenerateError(error));
    LogError(error);
}

void AppleUIFramework::Logging::Error::CreateErrorProgramPause(std::string title, std::string description, bool requires_verbose)
{
    Error error = GenerateError(title, description, requires_verbose);
    AppleUIFramework::Logging::log.push_back(EWL::GenerateError(error));
    LogError(error);

    std::cout << "Press Enter to continue...\n";
    GRABLOGFILE(std::print(file, "Press Enter to continue..."));
    std::cin.get();
}

void AppleUIFramework::Logging::Error::CreateErrorProgramExplode(std::string title, std::string description, bool requires_verbose)
{
    Error error = GenerateError(title, description, requires_verbose);
    AppleUIFramework::Logging::log.push_back(EWL::GenerateError(error));
    LogError(error);

    Explode();
}

auto AppleUIFramework::Logging::Log::GenerateLog(std::string title, std::string description, bool requires_verbose) -> AppleUIFramework::Logging::Log
{
    Log thislog;
    thislog.title = title;
    thislog.description = description;
    thislog.time = AppleUIFramework::Logging::GetFormattedTime();
    thislog.requires_verbose = requires_verbose;

    return thislog;
}

void AppleUIFramework::Logging::Log::LogLog(Log thislog)
{
    if (static_cast<int>(thislog.requires_verbose) <= static_cast<int>(verbose))
    {
        std::cout << rang::fg::blue << thislog.time << rang::fg::green << ": " << thislog.title << ":\n"
                  << thislog.description << "\n"
                  << rang::fg::reset;
        GRABLOGFILE(std::println(file, "{}: {}:\n{}", thislog.time, thislog.title, thislog.description));
    }
}

void AppleUIFramework::Logging::Log::CreateLog(std::string title, std::string description, bool requires_verbose)
{
    Log thislog = GenerateLog(title, description, requires_verbose);
    log.push_back(EWL::GenerateLog(thislog));
    LogLog(thislog);
}

auto AppleUIFramework::Logging::GetFormattedTime() -> std::string
{
    auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
    return std::format("[{:%Y-%m-%d %X}]", time);
}

void AppleUIFramework::Logging::InitLog()
{
    std::string log_file_name = log_file_beginning + std::regex_replace(GetFormattedTime(), std::regex(":"), ";") + ".txt";
    log_file = log_file_location / log_file_name;
    std::filesystem::create_directories(log_file_location);

    std::ofstream file(AppleUIFramework::Logging::log_file);
    if (!file.is_open())
    {
        // we call the longer method to avoid trying to write to the log again
        AppleUIFramework::Logging::Error::LogError(AppleUIFramework::Logging::Error::GenerateError("Failed to open \"" + log_file.string() + "\"", "Either failed to open \"" + log_file.string() + "\", or failed to create"), false);
    }

    std::println(file, "{}: Log Initalized!", GetFormattedTime());

    file.close();

    log_file_initalised = true;
}

auto AppleUIFramework::Logging::EWL::GenerateLog(Log log) -> AppleUIFramework::Logging::EWL
{
    EWL elboth;
    elboth.log = log;
    return elboth;
}

auto AppleUIFramework::Logging::EWL::GenerateWarning(Warning warning) -> AppleUIFramework::Logging::EWL
{
    EWL elboth;
    elboth.warning = warning;
    return elboth;
}

auto AppleUIFramework::Logging::EWL::GenerateError(Error error) -> AppleUIFramework::Logging::EWL
{
    EWL elboth;
    elboth.error = error;
    return elboth;
}

void AppleUIFramework::Logging::EWL::Print()
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
        Error::LogError(Error::GenerateError("I don't contain error or log or warning?", "Wait, I don't contain a log or an error?"));
    }
}

auto AppleUIFramework::Logging::Warning::GenerateWarning(std::string title, std::string description, bool requires_verbose, bool use_stacktrace,
                                                         bool use_stacktrace_verbose) -> AppleUIFramework::Logging::Warning
{
    Warning warning;
    warning.title = title;
    warning.description = description;
    warning.time = AppleUIFramework::Logging::GetFormattedTime();
    warning.requires_verbose = requires_verbose;
    if (use_stacktrace || (use_stacktrace_verbose && verbose))
    {
        auto stacktrace = std::stacktrace::current();
        warning.stacktrace = std::to_string(stacktrace);
    }

    return warning;
}

void AppleUIFramework::Logging::Warning::CreateWarning(std::string title, std::string description, bool requires_verbose, bool use_stacktrace, bool use_stacktrace_verbose)
{
    Warning warning = GenerateWarning(title, description, requires_verbose, use_stacktrace, use_stacktrace_verbose);
    log.push_back(EWL::GenerateWarning(warning));
    LogWarning(warning);
}

void AppleUIFramework::Logging::Warning::LogWarning(Warning thiswarning)
{
    if (static_cast<int>(thiswarning.requires_verbose) <= static_cast<int>(verbose))
    {
        std::cout << rang::fg::blue << thiswarning.time << rang::fg::yellow << ": " << thiswarning.title << ":\n"
                  << thiswarning.description << "\n";
        GRABLOGFILE(std::println(file, "{}: {}:\n{}", thiswarning.time, thiswarning.title, thiswarning.description));
        if (thiswarning.use_stacktrace || (thiswarning.use_stacktrace_verbose && verbose))
        {
            std::cout << "Stacktrace:\n"
                      << thiswarning.stacktrace << rang::fg::reset;
            GRABLOGFILE(std::println(file, "Stacktrace:{}", thiswarning.stacktrace));
        } else
        {
            std::cout << rang::fg::reset;
        }
    }
}

auto AppleUIFramework::Logging::Error::IsSameAs(Error thiserror) const -> bool
{
    return title == thiserror.title && description == thiserror.description;
}

auto AppleUIFramework::Logging::Warning::IsSameAs(Warning thiswarning) const -> bool
{
    return title == thiswarning.title && description == thiswarning.description;
}

auto AppleUIFramework::Logging::Log::IsSameAs(Log thislog) const -> bool
{
    return title == thislog.title && description == thislog.description;
}

auto AppleUIFramework::Logging::EWL::IsSame(AppleUIFramework::Logging::EWL thisewl) -> bool
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

void AppleUIFramework::Logging::SetupTerminator()
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
            Warning::CreateWarning("COULDN'T FIND UNHANDLED EXCEPTION WHAT()", "Simply couldn't find out what type it is. Not a defined type in AppleUIFramework::Logging::SetupTerminator", false, false, true);
        }
        std::abort();
    });
}