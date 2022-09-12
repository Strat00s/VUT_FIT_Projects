import './Login.css';
import React from 'react';

import Button from 'react-bootstrap/Button';
import Form from 'react-bootstrap/Form';
import Modal from 'react-bootstrap/Modal';
import Container from 'react-bootstrap/Container';
import Spinner from 'react-bootstrap/Spinner';
import InputGroup from 'react-bootstrap/InputGroup';
import Card from 'react-bootstrap/Card';
import axios from 'axios';
import { useHistory } from 'react-router-dom';
import { useState, useEffect } from 'react';

import bank_img from '../../Other/bank-1938641-1643400inverted.png';    //source: https://cdn.iconscout.com/icon/free/png-256/bank-1938641-1643400.png

//import jwt from 'jsonwebtoken';

function Login() {
    //main structure for passing data
    var data = {
        ip: '',
        token: undefined,
        expiry: undefined,
        user_data: undefined,
        currencies: undefined,
        balances: undefined,
        history: undefined,
        last_symbol: undefined,
        date: undefined,
        time: undefined,
        left: undefined
    };


    const history = useHistory();
    
    //login/register states
    const [username, setUsername] = useState('');
    const [password, setPassword] = useState('');
    const [confirmPassword, setConfirmPassword] = useState('');
    const [wantRegister, setWantRegister] = useState(false);
    const [passwordMatch, setPasswordMatch] = useState();

    //server status
    const [online, setOnline] = useState(false);
    const [pingLoading, setPingLoading] = useState(false);

    //Error popup
    const [errorPopup, setErrorPopup] = useState(false);
    const [errorMessage, setErrorMessage] = useState('');
    const [errorStyle, setErrorStyle] = useState('');
    const setError = (message, is_on, style) => {
        setErrorMessage(message);
        setErrorPopup(is_on);
        setErrorStyle(style);
    }
    
    const [server_address, setServerAddress] = useState("192.168.1.176:5000");
    const [loading, setLoading] = useState(0); //loading animation

    //change state
    const onUsernameChange = (e) => setUsername(e.target.value);
    const onPasswrodChange = (e) => setPassword(e.target.value);
    const onConfirmPChange = (e) => {
        setConfirmPassword(e.target.value);
        if (password === e.target.value) {setPasswordMatch(true);}
        else {setPasswordMatch(false)}
    }
    const onServerChange = (e) => {
        setServerAddress(e.target.value);
    }

    const handleEPopupClose = () => setErrorPopup(false);
    const handleWantRegister = () => setWantRegister(!wantRegister);

    //get server status the easy way
    const ping = async() => {
        setPingLoading(true);
        try{
            await axios.get("http://"+server_address+"/system", {headers: {"Content-Type": "application/json"}});
			setOnline(true);
        }
        catch(error){
            if (error.response) setOnline(true);
            else setOnline(false);
        }
        setPingLoading(false);
    }

    //ping on load
    useEffect(() => {
        ping();
    }, []);

    //TODO one function???
    const register = async(usr, passwd, passwd_confirm) => {
        setLoading(1);
        if (passwd !== passwd_confirm) {setError("Passwords do not match", true, "info");}
        else {
            try {
                let response = await axios.post("http://"+server_address+"/users", {'username': usr, 'password': passwd}, {"Content-Type": "application/json"} );   //auth
                
                //user exists
                if (response.data.status === "fail"){
                    setError(response.data.data.username, true, "warning");
                }
                else {
                    handleWantRegister();
                    setError("User "+username+" has been succesfully created. You can now login.", true, "success");
                }
            } 
            catch(error) {
                if (error.response) {setError("Password is too short", true, "info");}
                else {setError("Server is unavailable", true, "info")}
            }
        }
        setLoading(0);
    }

    const logIn = async(usr, passwd) => {
        setLoading(1);  //start loading animation
        try {
            //get and save all data
            let response = await axios.post("http://"+server_address+"/auth", {'username': usr, 'password': passwd}, {headers: {"Content-Type": "application/json"}});   //auth
            data.token = response.data.api_token;
            data.expiry = response.data.expiry;
            response = await axios.get("http://"+server_address+"/users/"+usr, {headers: {"Content-Type": "application/json"}});
            data.user_data = response.data;
            response = await axios.get("http://"+server_address+"/currencies", {headers: {"Content-Type": "application/json"}});
            data.currencies = response.data;
            response = await axios.get("http://"+server_address+"/currencies/balances", {headers: {"Content-Type": "application/json", "Authorization": "Bearer "+data.token}});
            data.balances = response.data;
            data.last_symbol = data.balances[0].symbol;
            response = await axios.get("http://"+server_address+"/currencies/"+data.balances[0].symbol+"/transactions/"+usr, {headers: {"Content-Type": "application/json", "Authorization": "Bearer "+data.token}});
            data.history = response.data;
            
            data.ip = server_address;   //save ip
            setLoading(0); //stop loading animation
            history.push({ pathname: '/home', data: data, page_num: 1 });    //go home
        } 
        catch(error) {
            if (error.response) {setError("Invalid credentials", true, "info");}
            else {setError("Server is unavailable", true, "warning");}
            setLoading(0); //stop loading animation
        }
    }

    return (
        <div className="LoginPage">
            <Modal className="rounded" size="sm" show={errorPopup} onHide={handleEPopupClose}>
                <Container className="bg-dark text-white border-0">
                    <div className="LPErrorModalMessage">
                        <Modal.Header>{errorMessage}</Modal.Header>
                    </div>
                </Container>
                <Modal.Footer className=" border-0 bg-dark text-white">
                    <Container>
                        <div className="LPErrorModalButton">
                            <Button variant={errorStyle} onClick={handleEPopupClose}>Close</Button>
                        </div>
                    </Container>
                </Modal.Footer>
            </Modal>
            
            <Container style={{"maxWidth": 400}}>
                <Card className="bg-dark text-white border rounded-0">
                    <Card.Header className="border-light">
                        <div className="LPBankLogo">
                            <img className="LPBankIamge" src={bank_img} alt="Bank" height="120"/>
                            <h1 >PonziBank</h1>
                        </div>
                    </Card.Header>
                    <Card.Body>
                        <h5 style={{"textAlign":"center"}}>Bank for your gaming needs</h5>
                    </Card.Body>
                </Card>
                <Card className="bg-dark text-white border rounded-0">
                    <div className="LPMainBody">
                    <div className="LPServerInput">
                        <Form>
                            <Form.Group as={Container}>
                                <InputGroup className="mb-3">
                                    <InputGroup.Prepend>
                                        <InputGroup.Text className="bg-secondary text-white" id="basic-addon1">Server IP:</InputGroup.Text>
                                    </InputGroup.Prepend>
                                    <Form.Control className="bg-dark text-white" style={{"width": 170}} placeholder={server_address} onChange={onServerChange} value={server_address}/>
                                    <InputGroup.Append>
                                        <Button className="border border-white" style={{"width": 48}} variant={online ? "success" : "danger"} onClick={ping}>
                                            {pingLoading === false && online === true && <div>OK</div>}
                                            {pingLoading === false && online === false && <div>X</div>}
                                            {pingLoading === true && <Spinner size="sm" animation="grow"/>}
                                        </Button>
                                    </InputGroup.Append>
                                </InputGroup>
                                <Form.Text style={{'fontSize': 12, "float":"right"}}>
                                    Check status
                                </Form.Text>
                            </Form.Group>
                        </Form>
                    </div>
        
                        <Form>
                            <Form.Group as={Container} controlId="LoginForm">
                                <Form.Label>Enter your credentials:</Form.Label>
                                <Form.Control className="bg-dark text-white" as="input" type="username" placeholder="Username" onChange={onUsernameChange} value={username}/>
                                <Form.Control className="bg-dark text-white" as="input" type="password" placeholder="Password" onChange={onPasswrodChange} value={password} isInvalid={password.length < 8 && wantRegister}/>
                                {wantRegister && 
                                    <Form.Text className="text-muted" style={{'fontSize': 12}}>
                                            At least 8 characters long.
                                    </Form.Text>
                                }
                            </Form.Group>
                            <Form.Group as={Container}>
                                <Form.Check checked={wantRegister} type="checkbox" label="I want to add a new user" onChange={() => handleWantRegister()} style={{'fontSize': 15}}/>
                            </Form.Group>
                            {wantRegister && 
                                <Form.Group as={Container}>
                                    <Form.Control className="bg-dark text-white" isValid={passwordMatch} isInvalid={!passwordMatch} as="input" type="password" placeholder="Confirm Password" onChange={onConfirmPChange} value={confirmPassword}/>
                                </Form.Group>
                            }
                        </Form>
                        <div className="LRButton">
                            {!wantRegister &&
                                <Button variant="primary" type="Submit" onClick={() => logIn(username, password)}>
                                    {loading === 0 && <div>Login</div>}
                                    {loading === 1 && <Spinner size="sm" animation="grow"/>}
                                </Button>
                            }
                            {wantRegister &&
                                <Button variant="primary" type="Submit" onClick={() => register(username, password, confirmPassword)}>
                                    {loading === 0 && <div>Register</div>}
                                    {loading === 1 && <Spinner size="sm" animation="grow"/>}
                                </Button>
                            }
                        </div>
                        </div>
                    </Card>
                </Container>
        </div>
    )
}

export default Login