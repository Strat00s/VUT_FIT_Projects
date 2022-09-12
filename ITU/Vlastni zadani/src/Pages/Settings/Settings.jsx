import React from 'react';
import TopBar from '../../Components/TopBar'

function Settings(props) {
    var data = props.location.data;

    function getData(e) {
        console.log(data);
    }

    return (
        <div>
            <TopBar page_num={4} data={data}/>
            <h1>
                Settings
            </h1>
            <button type="test" onClick={getData}>Click me!</button>
        </div>
    )
}

export default Settings