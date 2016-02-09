///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) Tavendo GmbH
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBAHN_SESSION_HPP
#define AUTOBAHN_SESSION_HPP

#include "wamp_call_options.hpp"
#include "wamp_call_result.hpp"
#include "wamp_subscribe_options.hpp"
#include "wamp_event_handler.hpp"
#include "wamp_message.hpp"
#include "wamp_procedure.hpp"

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#include <boost/asio.hpp>
#include <boost/thread/future.hpp>
#include <cstdint>
#include <functional>
#include <istream>
#include <ostream>
#include <map>
#include <memory>
#include <msgpack.hpp>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#if defined(_WIN32) || defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef ERROR
#undef ERROR
#endif

namespace autobahn {

class wamp_call;
class wamp_message;
class wamp_register_request;
class wamp_registration;
class wamp_subscribe_request;
class wamp_subscription;
class wamp_unsubscribe_request;
class wamp_authenticate;
class wamp_challenge;

/// Representation of a WAMP session.
template<typename IStream, typename OStream>
class wamp_session : public std::enable_shared_from_this<wamp_session<IStream, OStream>>
{
public:

    /*!
     * Create a new WAMP session.
     *
     * \param in The input stream to run this session on.
     * \param out THe output stream to run this session on.
     */
    wamp_session(boost::asio::io_service& io, IStream& in, OStream& out, bool debug = false);

    ~wamp_session();

    /*!
     * Start listening on the IStream provided to the constructor
     * of this session.
     *
     * \return A future that resolves to true if the session was successfully
     *         started and false otherwise.
     */
    boost::future<bool> start();

    /*!
     * Closes the IStream and the OStream provided to the constructor
     * of this session.
     */
    boost::future<void> stop();

    /*!
     * Join a realm with this session.
     *
     * \param realm The realm to join on the WAMP router connected to.
     * \param authmethods The authentication methods this instance support e.g. "wampcra","ticket"
     * \param authid The username or maybe an other identifier for the user to join.
     * \return A future that resolves with the session ID when the realm was joined.
     */
    boost::future<uint64_t> join(
            const std::string& realm,
            const std::vector<std::string>& authmethods = std::vector<std::string>(),
            const std::string& authid = "" );
    /*!
     * Leave the realm.
     *
     * \param reason An optional WAMP URI providing a reason for leaving.
     * \return A future that resolves with the reason sent by the peer.
     */
    boost::future<std::string> leave(const std::string& reason = std::string("wamp.error.close_realm"));

    /*!
     * Publish an event with empty payload to a topic.
     *
     * \param topic The URI of the topic to publish to.
     * \return A future that resolves once the the topic has been published to.
     */
    boost::future<void> publish(const std::string& topic);

    /*!
     * Publish an event with positional payload to a topic.
     *
     * \param topic The URI of the topic to publish to.
     * \param arguments The positional payload for the event.
     * \return A future that resolves once the the topic has been published to.
     */
    template <typename List>
    boost::future<void> publish(const std::string& topic, const List& arguments);

    /*!
     * Publish an event with both positional and keyword payload to a topic.
     *
     * \param topic The URI of the topic to publish to.
     * \param arguments The positional payload for the event.
     * \param kw_arguments The keyword payload for the event.
     * \return A future that resolves once the the topic has been published to.
     */
    template <typename List, typename Map>
    boost::future<void> publish(const std::string& topic, const List& arguments, const Map& kw_arguments);

    /*!
     * Subscribe a handler to a topic to receive events.
     *
     * \param topic The URI of the topic to subscribe to.
     * \param handler The handler that will receive events under the subscription.
     * \param options The options to pass in the subscribe to the router.
     * \return A future that resolves to a autobahn::subscription
     */
    boost::future<wamp_subscription> subscribe(
            const std::string& topic, const wamp_event_handler& handler,
            const wamp_subscribe_options& options = wamp_subscribe_options());

    /*!
     * Unubscribe a handler to previosuly subscribed topic.
     *
     * \param subscription The subscription to unsubscribe from.
     * \return A future that synchronizes to the unsubscribed response.
     */
    boost::future<void> unsubscribe(const wamp_subscription& subscription);

    /*!
     * Calls a remote procedure with no arguments.
     *
     * \param procedure The URI of the remote procedure to call.
     * \param options The options to pass in the call to the router.
     * \return A future that resolves to the result of the remote procedure call.
     */
    boost::future<wamp_call_result> call(
            const std::string& procedure,
            const wamp_call_options& options = wamp_call_options());

    /*!
     * Calls a remote procedure with positional arguments.
     *
     * \param procedure The URI of the remote procedure to call.
     * \param arguments The positional arguments for the call.
     * \param options The options to pass in the call to the router.
     * \return A future that resolves to the result of the remote procedure call.
     */
    template <typename List>
    boost::future<wamp_call_result> call(
            const std::string& procedure, const List& arguments,
            const wamp_call_options& options = wamp_call_options());

    /*!
     * Calls a remote procedure with positional and keyword arguments.
     *
     * \param procedure The URI of the remote procedure to call.
     * \param arguments The positional arguments for the call.
     * \param kw_arguments The keyword arguments for the call.
     * \param options The options to pass in the call to the router.
     * \return A future that resolves to the result of the remote procedure call.
     */
    template<typename List, typename Map>
    boost::future<wamp_call_result> call(
            const std::string& procedure, const List& arguments, const Map& kw_arguments,
            const wamp_call_options& options = wamp_call_options());

    /*!
     * Register an procedure as a procedure that can be called remotely.
     *
     * \param uri The URI under which the procedure is to be exposed.
     * \param procedure The procedure to be exposed as a remotely callable procedure.
     * \param options Options when registering a procedure.
     * \return A future that resolves to a autobahn::registration
     */
    boost::future<wamp_registration> provide(
            const std::string& uri,
            const wamp_procedure& procedure,
            const provide_options& options = provide_options());

    /*!
     * Function called by the session when authenting. It always has to be re-implemented (if authentication is part of the system).
     *
     * \param challenge The challenge from the router. Containing enough information, for the system to prove membership.
     * \return A future deliver a proper authentication response to the challenge given.
     */
    virtual boost::future<wamp_authenticate> on_challenge(const wamp_challenge& challenge);


private:
    void process_error(wamp_message&& message);
    void process_welcome(wamp_message&& message);
    void process_abort(wamp_message&& message);
    void process_challenge(wamp_message&& message);
    void process_call_result(wamp_message&& message);
    void process_subscribed(wamp_message&& message);
    void process_unsubscribed(wamp_message&& message);
    void process_event(wamp_message&& message);
    void process_registered(wamp_message&& message);
    void process_invocation(wamp_message&& message);
    void process_goodbye(wamp_message&& message);

    void send_message(wamp_message&& message, bool session_established = true);
    void receive_message();

    void got_handshake_reply(const boost::system::error_code& error);
    void got_message_header(const boost::system::error_code& error);
    void got_message_body(const boost::system::error_code& error);
    void got_message(wamp_message&& message);


    bool m_debug;

    boost::asio::io_service& m_io_service;

    /// Input stream this session runs on.
    IStream& m_in;

    /// Output stream this session runs on.
    OStream& m_out;

    char m_message_length_buffer[4];
    uint32_t m_message_length;

    /// MsgPack unserialization unpacker.
    msgpack::unpacker m_unpacker;

    /// Last request ID of outgoing WAMP requests.
    std::atomic<uint64_t> m_request_id;

    /// Buffer used to hold the sent/recevied rawsocket handshake
    char m_handshake_buffer[4];

    /// Synchronization for dealing with the rawsocket handshake
    boost::promise<bool> m_handshake;

    /// WAMP session ID (if the session is joined to a realm).
    uint64_t m_session_id;

    /// Future to be fired when session was joined.
    boost::promise<uint64_t> m_session_join;

    bool m_goodbye_sent;

    boost::promise<std::string> m_session_leave;

    /// Set to true when the session is stopped.
    bool m_running;

    /// Synchronization for dealing with stopping the session
    boost::promise<void> m_session_stop;

    //////////////////////////////////////////////////////////////////////////////////////
    /// Caller

    /// Map of outstanding WAMP calls (request ID -> call).
    std::map<uint64_t, std::shared_ptr<wamp_call>> m_calls;


    //////////////////////////////////////////////////////////////////////////////////////
    /// Subscriber

    /// Map of outstanding WAMP subscribe requests (request ID -> subscribe request).
    std::map<uint64_t, std::shared_ptr<wamp_subscribe_request>> m_subscribe_requests;

    /// Map of outstanding WAMP unsubscribe requests (request ID -> unsubscribe request).
    std::map<uint64_t, std::shared_ptr<wamp_unsubscribe_request>> m_unsubscribe_requests;

    /// Map of subscribed handlers (subscription ID -> handler)
    std::multimap<uint64_t, wamp_event_handler> m_subscription_handlers;


    //////////////////////////////////////////////////////////////////////////////////////
    /// Callee

    /// Map of outstanding WAMP register requests (request ID -> register request).
    std::map<uint64_t, std::shared_ptr<wamp_register_request>> m_register_requests;

    /// Map of registered procedures (registration ID -> procedure)
    std::map<uint64_t, wamp_procedure> m_procedures;
};

} // namespace autobahn

#include "wamp_session.ipp"

#endif // AUTOBAHN_SESSION_HPP
