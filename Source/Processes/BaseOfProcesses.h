
#pragma once
#include <adecc_Tools/MyTrace.h>
#include <string>
#include <format>

using namespace std::string_literals;

/**
 * \brief abstract interface for the business processes and subprocesses
*/
class TOperationsBase {
	public:
		TOperationsBase() { Trace("constructor for TOperationsBase called"s); }
		TOperationsBase(TOperationsBase const&) = delete;
		TOperationsBase(TOperationsBase&&) noexcept = delete;
		virtual ~TOperationsBase(void) = default;

		TOperationsBase& operator = (TOperationsBase const&) = delete;
		TOperationsBase& operator = (TOperationsBase&&) noexcept = delete;

		virtual std::string Application(void) const = 0;
		virtual std::string ApplicationVersion(void) const = 0;
		virtual std::string ApplicationText(void) const = 0;
   };


template<unsigned N>
struct MyHlpString {
	char buffer[N + 1]{};
	constexpr MyHlpString(char const* param) { for (unsigned i = 0; i <= N; ++i) buffer[i] = param[i]; }
	constexpr operator char const* () const { return buffer; }
};

template<unsigned N> MyHlpString(char const (&)[N]) -> MyHlpString<N - 1>;



template <typename sub_process_class, MyHlpString App, MyHlpString Version>
	requires std::is_base_of_v<TOperationsBase, sub_process_class>
class SubProcessConcrete : virtual public TOperationsBase, virtual public sub_process_class {
   public:
	   SubProcessConcrete(void) : TOperationsBase(), sub_process_class() { Trace("call constructor for SubProcessConcrete "s + std::string(App) + " ("s + std::string(Version) + ")"); }
	   virtual ~SubProcessConcrete() { }

	   SubProcessConcrete(SubProcessConcrete const&) = delete;
	   SubProcessConcrete(SubProcessConcrete&&) noexcept = delete;
	   SubProcessConcrete& operator = (SubProcessConcrete const&) = delete;
	   SubProcessConcrete& operator = (SubProcessConcrete&&) noexcept = delete;

	   virtual std::string Application(void) const override { return std::string(App); }
	   virtual std::string ApplicationVersion(void) const override { return std::string(Version); }
	   virtual std::string ApplicationText(void) const override { return Application() + " (" + ApplicationVersion() + ")"; }
};




/** \brief Concept process_type defined to check process classes to guarantee that all constraints that we formulate for
 * the classes which we want assemble to our business processes are really fulfilled.
 *
 * \details Classes that are assembled for the processes should be follow this constraints
 * - there is only an unique instance of a concrete process class
 * - (have a standard constructor)
 * - be a class
 * - be polymorphic
 * - have a virtual destructor
 * - not have a copy constructor
 * - not have a move constructor
 * - not have a copying assignment operator
 * - not have a move assignment operator
 *
 * \details Note that while an abstract class has a default constructor for its own data elements, it is not constructible.
 * Abstract must not be checked, because later concrete classes also are used as base classes for implementations
 * \details std::is_copy_constructible_v, std::is_move_constructible_v return false for abstract classes. Its not possible to
 * check the existence of the constructors with Standard C++, but its not necessary because its not callable for abstract classes
 * \tparam  ty the type should be tested with the concept process_type
 */
template <typename ty>
concept process_type = std::is_class_v<ty> &&
                       std::is_polymorphic_v<ty> &&
                       std::has_virtual_destructor_v<ty> &&
                       !std::is_copy_constructible_v<ty> &&
                       !std::is_move_constructible_v<ty> &&
                       !std::is_copy_assignable_v<ty> &&
                       !std::is_move_assignable_v<ty>;

template <typename ty>
concept concrete_process_type = process_type<ty> &&
                                !std::is_abstract_v<ty> &&
                                std::is_default_constructible_v<ty>;

template <typename ty>
using concrete_process = std::enable_if_t<concrete_process_type<ty>, ty>;


/** @brief A concept that checks if all arguments of the processing implementation type `args` satisfy the type `process_type`.
 * @tparam args pack of argument types for assembling the type for the process implementation.
 *
 * @see process_type
 */
template <typename... args>
concept process_impl_types = (process_type<args> && ...);


/**
 * @brief Eine Hilfsklasse, die von `proc_ty` und `proc_impl_types` erbt.
 *
 * @tparam proc_ty Der Typ der Prozessierungsschnittstelle.
 * @tparam proc_impl_types Die Liste der Verarbeitungsimplementierungstypen.
 */
template <process_type proc_ty, process_impl_types... proc_impl_types>
class TProcess_Helper : virtual public proc_ty, virtual public proc_impl_types... {
public:
	/// @brief base constructor for the help class TProcess_Helper
	TProcess_Helper() : proc_ty(), proc_impl_types()... { Trace("constructor for template TProcess_Helper called."s); }

	TProcess_Helper(TProcess_Helper const& ref) = delete;      //: proc_ty(ref), proc_impl_types(ref)... { }
	TProcess_Helper(TProcess_Helper&& rref) noexcept = delete; //: proc_ty(std::move(rref)), proc_impl_types(std::move(rref))... { }
	virtual ~TProcess_Helper() = default;

	TProcess_Helper& operator = (TProcess_Helper const& ref) = delete;
	TProcess_Helper& operator = (TProcess_Helper&& rref) noexcept = delete;
};



