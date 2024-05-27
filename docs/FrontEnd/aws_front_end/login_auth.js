function request_data(resource, on_success, on_error) {
    if (sessionStorage.getItem("uk_election_ad_key")) {
        // For the time being we force this into Text format to allow the CORS
        // policy to let the request through. Can be optimized when out of dev
        // ... but you know, the data's small, so we might not bother:)
        $.ajax({
            url: 'https://wgva0jxyv5.execute-api.eu-west-2.amazonaws.com/' + resource,
            type: "GET",
            dataType: 'text',
            headers: {
                "ad_api_auth_token": sessionStorage.getItem("uk_election_ad_key"),
                "ad_api_auth_user": sessionStorage.getItem("uk_election_ad_user")
            },
            success: (data) => {
                on_success(JSON.parse(data));
            },
            error: (response)  => {
                on_error(response.responseText)
            }
        })
    } else {
        on_error('No login credentials');
    }
}
function check_authorisation(on_success, on_error) {
    if (sessionStorage.getItem("uk_election_ad_key")) {
        request_data("hello_world", (data) => {
            console.log("We've got read access!")
            on_success();
        }, (error_message) => {
            clear_credentials();
            console.log("That didn't work")
            on_error(error_message)
        })
    } else {
        on_error('')
    }
}
function clear_credentials() {
    sessionStorage.removeItem("uk_election_ad_key");
    sessionStorage.removeItem("uk_election_ad_user");
}

function initiate_login(user, pass, on_success, on_error) {
    $.post({
        url: 'https://wgva0jxyv5.execute-api.eu-west-2.amazonaws.com/login',
        headers: {
            "ad_api_auth_user": user,
            "ad_api_auth_pass": pass
        },
        success: (data) => {
            sessionStorage.setItem("uk_election_ad_key", data.ad_api_auth_token);
            sessionStorage.setItem("uk_election_ad_user", user);
            check_authorisation(on_success, on_error);
        },
        error: (response)  => {
            on_error(response.responseText)
        }
    });
}

function logout(cb) {
    sessionStorage.removeItem("uk_election_ad_key");
    cb()
}

function AWSFrontEnd() {
    this.login_modal = {
        bootstrap_modal: new bootstrap.Modal('#login_modal', {
                focus: true,
                keyboard: false,
                backdrop: "static"
        }),
        user_name: $('#user_name'),
        password: $('#user_password'),
        submit_button: $('#login_button'),
        login_error_msg: $('#login_error_msg')
    };
    this.navbar = {
        jquery_object: $('nav.navbar'),
        logout_button: $('#logout_button')
    }
    const self = this;
    this.login_modal.submit_button.on("click", function () {
        initiate_login(self.login_modal.user_name.val(),
                       self.login_modal.password.val(),
                   () => self.login_successful(),
                    (error_text) => self.login_error(error_text))
        self.login_modal.password.val('')
    });
    this.navbar.logout_button.on("click", function () {
        logout(() => self.show_login_prompt_if_required());
    });
}
AWSFrontEnd.prototype = {
    login_successful: function() {
        this.login_modal.bootstrap_modal.hide();
    },
    login_error: function(errorText) {
        if (errorText) {
            this.login_modal.login_error_msg.text(errorText);
            this.login_modal.login_error_msg.show();
        } else {
            this.login_modal.login_error_msg.hide();
        }
        this.login_modal.bootstrap_modal.show();
    },
    show_login_prompt_if_required: function () {
        const self = this;
        check_authorisation(() => {
            self.login_successful();
        }, (errorText) => {
            self.login_error(errorText);
        });

    }
}