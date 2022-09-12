import React from 'react';
import './App.css';

import {Route, BrowserRouter as Router} from 'react-router-dom';

import Login from './Pages/Loginform/Login'
import Home from './Pages/Home/Home'
import History from './Pages/History/History'
import Payments from './Pages/Payments/Payments'

function App() {

    return (
        <div className="MainBody">
            <Router>
                <Route path="/login" component={Login} default_token={undefined}/>
                <Route path="/home" component={Home}/>
                <Route path="/payments" component={Payments}/>
                <Route path="/history" component={History}/>
            </Router>
        </div>
    );
}

export default App;
